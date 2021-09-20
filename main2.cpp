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

#define OFFSET_X 125
#define OFFSET_Y 25
#define SCREEN_SIZE 550

using namespace std;

// Função matemática a ser desenhada
string expression_string;
// Variável para "x"
double x_variable = 0;
// Controle de erros
int err;
// Ponteiro para expressão compilada
te_expr *expr;

int zoom = 1;

int x_centro = SCREEN_SIZE / 2 + OFFSET_X;
int y_centro = SCREEN_SIZE / 2 + OFFSET_Y;

// Solicita ao usuário que informe uma função
void obtemFuncao()
{
    cout << "Informe a equação a ser desenhada: ";
    getline(cin, expression_string);
    cout << "A equação será : " << expression_string << endl;
}

// Compila a expressão e registra as variáveis
void preparaFuncao()
{
    // Vincula o valor da variável global "x_variable" à variável "x" da expressão
    te_variable vars[] = {"x", &x_variable};

    /* Compile the expression with variables. */
    expr = te_compile(expression_string.c_str(), vars, 1, &err);
}

// Estrutura para representar pontos
typedef struct
{
    int x, y;
} Point;

// Variáveis necessárias para o SDL
unsigned int *pixels;
int width, height;
SDL_Surface *window_surface;
SDL_Renderer *renderer;

// Título da janela
std::string titulo = "SDL Points";

// Valores RGB para a cor de fundo da janela
const int VERMELHO = 0;
const int VERDE = 0;
const int AZUL = 0;

// Gera uma estrutura Point a partir de valores para x e y
Point getPoint(int x, int y)
{
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

// Obtém a cor de um pixel de uma determinada posição
Uint32 getPixel(int x, int y)
{
    if ((x >= 0 && x <= width) && (y >= 0 && y <= height))
        return pixels[x + width * y];
    else
        return -1;
}

// Seta um pixel em uma determinada posição,
// através da coordenadas de cor r, g, b, e alpha (transparência)
// r, g, b e a variam de 0 até 255
void setPixel(int x, int y, int r, int g, int b, int a)
{
    pixels[x + y * width] = SDL_MapRGBA(window_surface->format, r, g, b, a);
}

// Seta um pixel em uma determinada posição,
// através da coordenadas de cor r, g e b
// r, g, e b variam de 0 até 255
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

// Mostra na barra de título da janela a posição
// corrente do mouse
void showMousePosition(SDL_Window *window, int x, int y)
{
    std::stringstream ss;
    ss << titulo << " X: " << x << " Y: " << y;
    SDL_SetWindowTitle(window, ss.str().c_str());
}

// Imprime na console a posição corrente do mouse
void printMousePosition(int x, int y)
{
    printf("Mouse on x = %d, y = %d\n", x, y);
}

// Seta um pixel em uma determinada posição,
// através de um Uint32 representando
// uma cor RGB
void setPixel(int x, int y, Uint32 color)
{
    if ((x < 0 || x >= width || y < 0 || y >= height))
    {
        printf("Coordenada inválida : (%d,%d)\n", x, y);
        return;
    }
    pixels[x + y * width] = color;
}

// Retorna uma cor RGB(UInt32) formada
// pelas componentes r, g, b e a(transparência)
// informadas. r, g, b e a variam de 0 até 255
Uint32 RGB(int r, int g, int b, int a)
{
    return SDL_MapRGBA(window_surface->format, r, g, b, a);
}

// Retorna uma cor RGB(UInt32) formada
// pelas componentes r, g, e b
// informadas. r, g e b variam de 0 até 255
// a transparência é sempre 255 (imagem opaca)
Uint32 RGB(int r, int g, int b)
{
    return SDL_MapRGBA(window_surface->format, r, g, b, 255);
}

// Retorna um componente de cor de uma cor RGB informada
// aceita os parâmetros 'r', 'R','g', 'G','b' e 'B',
Uint8 getColorComponent(Uint32 pixel, char component)
{

    Uint32 mask;

    switch (component)
    {
    case 'b':
    case 'B':
        mask = RGB(0, 0, 255);
        pixel = pixel & mask;
        break;
    case 'r':
    case 'R':
        mask = RGB(255, 0, 0);
        pixel = pixel & mask;
        pixel = pixel >> 16;
        break;
    case 'g':
    case 'G':
        mask = RGB(0, 255, 0);
        pixel = pixel & mask;
        pixel = pixel >> 8;
        break;
    }
    return (Uint8)pixel;
}

// Aqui ocorrem as chamadas das funções a ser exibidas na janela
void display_linhas_do_plano()
{
    // y
    for (int y = OFFSET_Y; y <= 550 + OFFSET_Y; y += 10)
    {
        bresenham(OFFSET_X, y, width - OFFSET_X, y, 0, 255, 0);
    }

    //x
    for (int x = OFFSET_X; x <= 550 + OFFSET_X; x += 10)
    {
        bresenham(x, OFFSET_Y, x, height - OFFSET_Y, 0, 255, 0);
    }
}

void print_main_vect(int zoom_x, int zoom_y)
{
    bresenham(OFFSET_X + (SCREEN_SIZE / 2) + zoom_x, OFFSET_Y, OFFSET_X + (SCREEN_SIZE / 2) + zoom_x, OFFSET_Y + SCREEN_SIZE - 1, 0, 0, 255);
    bresenham(OFFSET_X, OFFSET_Y + (SCREEN_SIZE / 2) + zoom_y, OFFSET_X + SCREEN_SIZE - 1, OFFSET_Y + (SCREEN_SIZE / 2) + zoom_y, 0, 0, 255);
}

int main(int, char **)
{

    // o intervalo entre os pontos (Quanto maior o denominador, mais pontos, melhor o gráfico. Use 10, 100, 1000...)
    double delta = 1.0 / 10.0;
    // O intervalo de valores para x, que vai desde xumin até xmax (Sistema de Coordenadas do Universo: SRU)
    double xumin = -10.0;
    double xumax = 10.0;

    // obtemFuncao();
    // preparaFuncao();

    for (double x = xumin; x < xumax; x += delta)
    {

        double calculed_value;

        // Coloca o valor da variável x na global "x_variable", que está vinculada à expressão
        x_variable = x;
        // avalia a expressão para x
        calculed_value = te_eval(expr);

        // Aqui mostra o valor ou manda desenhar o pixel
        printf("%19.15f\t%19.15f\n", x, calculed_value);
    }

    std::cout << "Hello, world!\n";
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

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    window_surface = SDL_GetWindowSurface(window);
                    pixels = (unsigned int *)window_surface->pixels;
                    width = window_surface->w;
                    height = window_surface->h;
                    printf("Size changed: %d, %d\n", width, height);
                }
            }

            // Se o mouse é movimentado
            if (event.type == SDL_MOUSEMOTION)
            {
                // Mostra as posições x e y do mouse
                showMousePosition(window, event.motion.x, event.motion.y);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                /*Se o botão esquerdo do mouse é pressionado */
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    printf("Mouse pressed on (%d,%d)\n", event.motion.x, event.motion.y);
                }
            }
        }

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                setPixel(x, y, RGB(0, 0, 0));
            }
        }

        display_linhas_do_plano();
        print_main_vect(0, 0);
        SDL_UpdateWindowSurface(window);
    }
}
