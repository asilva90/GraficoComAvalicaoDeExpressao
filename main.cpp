#define SDL_MAIN_HANDLED
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stack>
#include <sstream>
#include <unistd.h>
#include "tinyexpr.h"

using namespace std;

typedef struct
{
    int x, y;
} Point;
typedef struct
{
    Point pIni, pFim;
} Rectangle;

// Variáveis necessárias para o SDL
unsigned int *pixels;
int width, height;
SDL_Surface *window_surface;
SDL_Renderer *renderer;

// Título da janela
std::string titulo = "SDL Graficos Matematicos";

Point getPoint(int x, int y)
{
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

Rectangle getRectangle(Point pIni, Point pFim)
{
    Rectangle r;
    r.pIni = pIni;
    r.pFim = pFim;
    return r;
}

// Parâmetros
const int WINDOWS_WIDTH = 800;
const int WINDOWS_HEIGHT = 600;
const int GRAPH_SIZE = 550;
const int GRID_MESH = 5;
const Point P1_SRU = getPoint(-10, -10);
const Point P2_SRU = getPoint(10, 10);

//Variáveis Globais
Rectangle graphSRU;
Rectangle graphSRD;
double escalaX;
double escalaY;

// Função matemática a ser desenhada
string expression_string;
// Variável para "x"
double x_variable = 0;
// Controle de erros
int err;
// Ponteiro para expressão compilada
te_expr *expr;

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

void setPixel(int x, int y, Uint32 color)
{
    if ((x < 0 || x >= width || y < 0 || y >= height))
    {
        printf("Coordenada invalida: (%d, %d)\n", x, y);
        return;
    }
    pixels[x + y * width] = color;
}

void setPixel(Point p, Uint32 color)
{
    setPixel(p.x, p.y, color);
}

void setPixelGraph(int x, int y, Uint32 color)
{
    if ((x < graphSRD.pIni.x || x >= graphSRD.pFim.x || y < graphSRD.pIni.y || y >= graphSRD.pFim.y))
    {
        return;
    }
    setPixel(x, y, color);
}

void setPixelGraph(Point p, Uint32 color)
{
    setPixelGraph(p.x, p.y, color);
}

Uint32 RGB(int r, int g, int b, int a)
{
    return SDL_MapRGBA(window_surface->format, r, g, b, a);
}

Uint32 RGB(int r, int g, int b)
{
    return RGB(r, g, b, 255);
}

void drawLineBresenham(int x1, int y1, int x2, int y2, Uint32 color)
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
        setPixel(x, y, color);
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
            setPixel(x, y, color);
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
        setPixel(x, y, color);
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
            setPixel(x, y, color);
        }
    }
}

void drawLineBresenham(Point p1, Point p2, Uint32 color)
{
    drawLineBresenham(p1.x, p1.y, p2.x, p2.y, color);
}

void drawRectangle(int x1, int y1, int x2, int y2, Uint32 color)
{
    drawLineBresenham(x1, y1, x2, y1, color);
    drawLineBresenham(x1, y1, x1, y2, color);
    drawLineBresenham(x2, y2, x1, y2, color);
    drawLineBresenham(x2, y2, x2, y1, color);
}

void drawRectangle(Point p1, Point p2, Uint32 color)
{
    drawRectangle(p1.x, p1.y, p2.x, p2.y, color);
}

void drawRectangle(Rectangle r, Uint32 color)
{
    drawRectangle(r.pIni, r.pFim, color);
}

Rectangle createGraphSRD(int graph_size)
{
    if (graph_size >= height || graph_size >= width)
    {
        printf("Grafico maior que a janela.");
        return getRectangle(getPoint(0, 0), getPoint(width - 1, height - 1));
    }

    int xc = width / 2;
    int yc = height / 2;
    int margem = graph_size / 2;
    return getRectangle(getPoint(xc - margem, yc - margem), getPoint(xc + margem, yc + margem));
}

Rectangle createGraphSRU(Point p1, Point p2)
{
    return getRectangle(p1, p2);
};

Point getPointSRUtoSRD(double x, double y)
{
    escalaX = double(graphSRD.pFim.x - graphSRD.pIni.x) / double(graphSRU.pFim.x - graphSRU.pIni.x);
    escalaY = double(graphSRD.pFim.y - graphSRD.pIni.y) / double(graphSRU.pFim.y - graphSRU.pIni.y);
    int distZeroX = 0 - graphSRU.pIni.x;
    int distZeroY = 0 - graphSRU.pIni.y;

    Point p;
    p.x = (x + distZeroX) * escalaX + graphSRD.pIni.x;
    p.y = (y + distZeroY) * (-escalaY) + graphSRD.pFim.y;
    return p;
}

void drawFunction()
{
    double y;
    for (double x = graphSRU.pIni.x; x <= graphSRU.pFim.x; x += 0.0001)
    {
        x_variable = x;
        // avalia a expressão para x
        y = te_eval(expr);

        setPixelGraph(getPointSRUtoSRD(x, y), RGB(255, 0, 0));
    }
}

void drawGrid(int grid_mesh)
{
    Uint32 colorGrid = RGB(0, 255, 0);

    for (int x = graphSRU.pIni.x + 1; x < graphSRU.pFim.x; x++)
    {
        if (x == 0 || abs(x) % grid_mesh == 0)
        {
            drawLineBresenham(getPointSRUtoSRD(x, graphSRU.pIni.y), getPointSRUtoSRD(x, graphSRU.pFim.y), colorGrid);
        }
    }

    for (int y = graphSRU.pIni.y + 1; y < graphSRU.pFim.y; y++)
    {
        if (y == 0 || abs(y) % grid_mesh == 0)
        {
            drawLineBresenham(getPointSRUtoSRD(graphSRU.pIni.x, y), getPointSRUtoSRD(graphSRU.pFim.x, y), colorGrid);
        }
    }
}

void drawBanana()
{
    drawLineBresenham(getPointSRUtoSRD(0, graphSRU.pIni.y), getPointSRUtoSRD(0, graphSRU.pFim.y), RGB(0, 0, 255));
    drawLineBresenham(getPointSRUtoSRD(graphSRU.pIni.x, 0), getPointSRUtoSRD(graphSRU.pFim.x, 0), RGB(0, 0, 255));
}

void graphMoveDown(int deslocamento)
{
    graphSRU.pIni.y -= deslocamento;
    graphSRU.pFim.y -= deslocamento;
}

void graphMoveUp(int deslocamento)
{
    graphSRU.pIni.y += deslocamento;
    graphSRU.pFim.y += deslocamento;
}

void graphMoveLeft(int deslocamento)
{
    graphSRU.pIni.x -= deslocamento;
    graphSRU.pFim.x -= deslocamento;
}

void graphMoveRight(int deslocamento)
{
    graphSRU.pIni.x += deslocamento;
    graphSRU.pFim.x += deslocamento;
}

void graphZoomMore()
{
    if (graphSRU.pFim.x - graphSRU.pIni.x <= 2 || graphSRU.pFim.y - graphSRU.pIni.y <= 2)
    {
        return;
    }
    graphSRU.pIni.x += 1;
    graphSRU.pIni.y += 1;
    graphSRU.pFim.x -= 1;
    graphSRU.pFim.y -= 1;
}

void graphZoomMinus()
{
    graphSRU.pIni.x -= 1;
    graphSRU.pIni.y -= 1;
    graphSRU.pFim.x += 1;
    graphSRU.pFim.y += 1;
}

void drawGraph()
{

    drawGrid(GRID_MESH);
    drawFunction();
    drawRectangle(graphSRD, RGB(0, 255, 0));
    drawBanana();
}

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

// Inicializa o SDL, abre a janela e controla o loop
// principal do controle de eventos
int main()
{
    // o intervalo entre os pontos (Quanto maior o denominador, mais pontos, melhor o gráfico. Use 10, 100, 1000...)
    double delta = 1.0 / 10.0;
    // O intervalo de valores para x, que vai desde xumin até xmax (Sistema de Coordenadas do Universo: SRU)
    double xumin = -10.0;
    double xumax = 10.0;

    obtemFuncao();
    preparaFuncao();

    // Inicializações iniciais obrigatórias
    setlocale(LC_ALL, NULL);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(titulo.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOWS_WIDTH, WINDOWS_HEIGHT, SDL_WINDOW_RESIZABLE);
    window_surface = SDL_GetWindowSurface(window);
    pixels = (unsigned int *)window_surface->pixels;
    width = window_surface->w;
    height = window_surface->h;

    printf("Pixel format: %s\n", SDL_GetPixelFormatName(window_surface->format->format));

    //Cria os gráficos SRU e SRD
    graphSRD = createGraphSRD(GRAPH_SIZE);
    graphSRU = createGraphSRU(P1_SRU, P2_SRU);
    int ctrlPressed = 0;
    int mousePressed = 0;
    Point pointPressed;

    while (1)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                exit(0);

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
                if (mousePressed)
                {
                    int deslocamentoX = (pointPressed.x - event.motion.x) / escalaX;
                    int deslocamentoY = (pointPressed.y - event.motion.y) / escalaY;

                    if (deslocamentoX > 0)
                    {
                        pointPressed = getPoint(event.motion.x, event.motion.y);
                        graphMoveRight(deslocamentoX);
                    }

                    if (deslocamentoX < 0)
                    {
                        pointPressed = getPoint(event.motion.x, event.motion.y);
                        graphMoveLeft(abs(deslocamentoX));
                    }

                    if (deslocamentoY > 0)
                    {
                        pointPressed = getPoint(event.motion.x, event.motion.y);
                        graphMoveDown(deslocamentoY);
                    }

                    if (deslocamentoY < 0)
                    {
                        pointPressed = getPoint(event.motion.x, event.motion.y);
                        graphMoveUp(abs(deslocamentoY));
                    }
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP)
            {
                mousePressed = 0;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                /*Se o botão esquerdo do mouse é pressionado */
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mousePressed = 1;
                    pointPressed = getPoint(event.motion.x, event.motion.y);
                    cout << "Mouse pressed on(" << event.motion.x << "," << event.motion.y << ")\n";
                }
            }

            if (event.type == SDL_MOUSEWHEEL)
            {
                if (ctrlPressed)
                {
                    if (event.wheel.y > 0)
                    { // scroll up
                        graphZoomMore();
                    }
                    else if (event.wheel.y < 0)
                    { // scroll down
                        graphZoomMinus();
                    }
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LCTRL:
                    ctrlPressed = 0;
                    break;
                case SDLK_RCTRL:
                    ctrlPressed = 0;
                    break;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LCTRL:
                    ctrlPressed = 1;
                    break;
                case SDLK_RCTRL:
                    ctrlPressed = 1;
                    break;
                case SDLK_LEFT:
                    graphMoveLeft(1);
                    printf("Pressionada a tecla LEFT\n");
                    break;
                case SDLK_a:
                    graphMoveLeft(1);
                    printf("Pressionada a tecla a\n");
                    break;
                case SDLK_RIGHT:
                    graphMoveRight(1);
                    printf("Pressionada a tecla RIGHT\n");
                    break;
                case SDLK_d:
                    graphMoveRight(1);
                    printf("Pressionada a tecla d\n");
                    break;
                case SDLK_UP:
                    graphMoveUp(1);
                    printf("Pressionada a tecla UP\n");
                    break;
                case SDLK_w:
                    graphMoveUp(1);
                    printf("Pressionada a tecla w\n");
                    break;
                case SDLK_DOWN:
                    graphMoveDown(1);
                    printf("Pressionada a tecla DOWN\n");
                    break;
                case SDLK_s:
                    graphMoveDown(1);
                    printf("Pressionada a tecla s\n");
                    break;
                case SDLK_z:
                    graphZoomMinus();
                    printf("Pressionada a tecla z\n");
                    break;
                case SDLK_x:
                    graphZoomMore();
                    printf("Pressionada a tecla x\n");
                    break;
                case SDLK_PLUS:
                    graphZoomMore();
                    printf("Pressionada a tecla PLUS\n");
                    break;
                case SDLK_MINUS:
                    graphZoomMinus();
                    printf("Pressionada a tecla MINUS\n");
                    break;
                case SDLK_KP_PLUS:
                    graphZoomMore();
                    printf("Pressionada a tecla KP_PLUS\n");
                    break;
                case SDLK_KP_MINUS:
                    graphZoomMinus();
                    printf("Pressionada a tecla KP_MINUS\n");
                    break;
                default:
                    break;
                }
            }
        }

        // Seta a cor de fundo da janela
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                setPixel(x, y, RGB(0, 0, 0));

        //desenha o gráfico
        drawGraph();

        SDL_UpdateWindowSurface(window);
    }
}