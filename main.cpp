#define SDL_MAIN_HANDLED
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stack>
#include <sstream>
#include <unistd.h>
#include "tinyexpr.h"

#define SCREEN_SIZE 550

#define init_x 125
#define init_y 25

#define end_x 675
#define end_y 575

#define init_intervalo -10
#define end_intervalo 10

using namespace std;

string expression_string;
double x_variable = 0;
int err;
te_expr *expr;

void obtemFuncao()
{
    cout << "Informe a equação a ser desenhada: ";
    getline(cin, expression_string);
    cout << "A equação será : " << expression_string << endl;
}

void preparaFuncao()
{
    te_variable vars[] = {"x", &x_variable};
    expr = te_compile(expression_string.c_str(), vars, 1, &err);
}

typedef struct
{
    int x, y;
} Point;

unsigned int *pixels;
int width, height;
SDL_Surface *window_surface;
SDL_Renderer *renderer;

int x_intervalo_init = init_intervalo;
int y_intervalo_init = init_intervalo;

int x_intervalo_end = end_intervalo;
int y_intervalo_end = end_intervalo;

std::string titulo = "GraficoComAvalicaoDeExpressao";

void setPixel(int x, int y, int r, int g, int b, int a)
{
    if ((x < 0 || x >= width || y < 0 || y >= height))
    {
        printf("Coordenada inválida : (%d,%d)\n", x, y);
        return;
    }

    pixels[x + y * width] = SDL_MapRGBA(window_surface->format, r, g, b, a);
}

void setPixel(int x, int y, int r, int g, int b)
{
    setPixel(x, y, r, g, b, 255);
}

void bresenham(int x1, int y1, int x2, int y2, int r, int g, int b)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

    dx = x2 - x1;
    dy = y2 - y1;

    dx1 = fabs(dx);
    dy1 = fabs(dy);

    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;

    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = x1;
            y = y1;
            xe = x2;
        }
        else
        {
            x = x2;
            y = y2;
            xe = x1;
        }
        setPixel(x, y, r, g, b);
        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
            {
                px = px + 2 * dy1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    y = y + 1;
                }
                else
                {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            setPixel(x, y, r, g, b);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x1;
            y = y1;
            ye = y2;
        }
        else
        {
            x = x2;
            y = y2;
            ye = y1;
        }
        setPixel(x, y, r, g, b);
        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
            {
                py = py + 2 * dx1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    x = x + 1;
                }
                else
                {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            setPixel(x, y, r, g, b);
        }
    }
}

void bresenham(int x1, int y1, int x2, int y2)
{
    bresenham(x1, y1, x2, y2, 0, 0, 0);
}

// Pegua um ponto do grafio e converte ele para um ponto da tela
Point graphPointToPoint(double x, double y)
{
    double graphX = (end_x - init_x) / (x_intervalo_end - x_intervalo_init);
    double graphY = (end_y - init_y) / (y_intervalo_end - y_intervalo_init);

    Point point;

    point.x = (x - x_intervalo_init) * graphX + init_x;
    point.y = (y - y_intervalo_init) * (-graphY) + end_y;
    return point;
}

void drawWindow()
{
    Point start;
    Point end;

    //Desenha as bordas da tela
    bresenham(init_x, init_y + 10, init_x, end_y, 0, 255, 0);
    bresenham(init_x, init_y + 10, end_x - 10, init_y + 10, 0, 255, 0);

    bresenham(end_x - 10, init_y + 10, end_x - 10, end_y, 0, 255, 0);
    bresenham(init_x, end_y, end_x - 10, end_y, 0, 255, 0);

    // Desenha as linhas
    for (int x = x_intervalo_init + 1; x < x_intervalo_end; x++)
    {
        if (x == 0 || abs(x) % 2 == 0)
        {
            start = graphPointToPoint(x, y_intervalo_init);
            end = graphPointToPoint(x, y_intervalo_end);
            bresenham(start.x, start.y, end.x, end.y, 0, 255, 0);
        }
    }
    for (int y = y_intervalo_init + 1; y < y_intervalo_end; y++)
    {
        if (y == 0 || abs(y) % 2 == 0)
        {
            start = graphPointToPoint(x_intervalo_init, y);
            end = graphPointToPoint(x_intervalo_end, y);
            bresenham(start.x, start.y, end.x, end.y, 0, 255, 0);
        }
    }

    // Desenha as linhas azuis
    start = graphPointToPoint(0, y_intervalo_init);
    end = graphPointToPoint(0, y_intervalo_end);

    if (!(start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) && !(end.x < 0 || end.x >= width || end.y < 0 || end.y >= height))
    {
        bresenham(start.x, start.y, end.x, end.y, 0, 0, 255);
    }

    start = graphPointToPoint(x_intervalo_init, 0);
    end = graphPointToPoint(x_intervalo_end, 0);

    if (!(start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) && !(end.x < 0 || end.x >= width || end.y < 0 || end.y >= height))
    {
        bresenham(start.x, start.y, end.x, end.y, 0, 0, 255);
    }

    // Desenha a função
    Point point_ant;
    for (double x = x_intervalo_init; x <= x_intervalo_end; x += 0.1)
    {
        x_variable = x;
        double y = te_eval(expr);

        Point point = graphPointToPoint(x, y);

        if ((point.x < init_x || point.x >= end_x || point.y < init_y || point.y >= end_y))
        {
            point_ant.x = NULL;
            point_ant.y = NULL;
            continue;
        }

        if (point_ant.x != NULL)
        {
            bresenham(point.x, point.y, point_ant.x, point_ant.y, 255, 0, 0);
            point_ant = point;
        }
        else
        {
            point_ant = point;
        }
    }
}

void handleEvents(SDL_Window *window)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
            case SDLK_a:
                x_intervalo_init -= 1;
                x_intervalo_end -= 1;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                x_intervalo_init += 1;
                x_intervalo_end += 1;
                break;
            case SDLK_UP:
            case SDLK_w:
                y_intervalo_init += 1;
                y_intervalo_end += 1;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                y_intervalo_init -= 1;
                y_intervalo_end -= 1;
                break;
            case SDLK_PLUS:
            case SDLK_KP_PLUS:
                if (x_intervalo_end - x_intervalo_init <= 2 || y_intervalo_end - y_intervalo_init <= 2)
                    break;
                x_intervalo_init += 1;
                y_intervalo_init += 1;
                x_intervalo_end -= 1;
                y_intervalo_end -= 1;
                break;
            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                x_intervalo_init -= 1;
                y_intervalo_init -= 1;
                x_intervalo_end += 1;
                y_intervalo_end += 1;
                break;
            default:
                break;
            }
        }
    }
}

void clean()
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            setPixel(x, y, 0, 0, 0);
        }
    }
}

int main(int, char **)
{
    obtemFuncao();
    preparaFuncao();

    SDL_Window *window = SDL_CreateWindow(titulo.c_str(),
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_RESIZABLE);

    window_surface = SDL_GetWindowSurface(window);

    pixels = (unsigned int *)window_surface->pixels;
    width = window_surface->w;
    height = window_surface->h;

    while (1)
    {
        handleEvents(window);

        clean();

        drawWindow();
        SDL_UpdateWindowSurface(window);
    }
}
