#define SDL_MAIN_HANDLED
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stack>
#include <sstream>
#include <unistd.h>

using namespace std;

// Estrutura para representar pontos
typedef struct {int x, y;} Point;

// Variáveis necessárias para o SDL
unsigned int * pixels;
int width, height;
SDL_Surface * window_surface;
SDL_Renderer * renderer;

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
    if((x>=0 && x<=width) && (y>=0 && y<=height))
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

// Mostra na barra de título da janela a posição
// corrente do mouse
void showMousePosition(SDL_Window * window, int x, int y)
{
    std::stringstream ss;
    ss << titulo << " X: " << x << " Y: " << y;
    SDL_SetWindowTitle(window, ss.str().c_str());
}

// Imprime na console a posição corrente do mouse
void printMousePosition(int x, int y)
{
    printf("Mouse on x = %d, y = %d\n",x,y);
}

// Seta um pixel em uma determinada posição,
// através de um Uint32 representando
// uma cor RGB
void setPixel(int x, int y, Uint32 color)
{
    if((x<0 || x>=width || y<0 || y>=height)) {
        printf("Coordenada inválida : (%d,%d)\n",x,y);
        return;
    }
    pixels[x + y * width] = color;
}

// Retorna uma cor RGB(UInt32) formada
// pelas componentes r, g, b e a(transparência)
// informadas. r, g, b e a variam de 0 até 255
Uint32 RGB(int r, int g, int b, int a) {
    return SDL_MapRGBA(window_surface->format, r, g, b, a);
}

// Retorna uma cor RGB(UInt32) formada
// pelas componentes r, g, e b
// informadas. r, g e b variam de 0 até 255
// a transparência é sempre 255 (imagem opaca)
Uint32 RGB(int r, int g, int b) {
    return SDL_MapRGBA(window_surface->format, r, g, b, 255);
}

// Retorna um componente de cor de uma cor RGB informada
// aceita os parâmetros 'r', 'R','g', 'G','b' e 'B',
Uint8 getColorComponent( Uint32 pixel, char component ) {

    Uint32 mask;

    switch(component) {
        case 'b' :
        case 'B' :
            mask = RGB(0,0,255);
            pixel = pixel & mask;
            break;
        case 'r' :
        case 'R' :
            mask = RGB(255,0,0);
            pixel = pixel & mask;
            pixel = pixel >> 16;
            break;
        case 'g' :
        case 'G' :
            mask = RGB(0,255,0);
            pixel = pixel & mask;
            pixel = pixel >> 8;
            break;
    }
    return (Uint8) pixel;
}

// Aqui ocorrem as chamadas das funções a ser exibidas na janela
void display()
{
    setPixel( 290, 240, RGB(0,0,0) );
    setPixel( 300, 240, RGB(255,0,0) );
    setPixel( 310, 240, RGB(255,255,0) );
    setPixel( 320, 240, RGB(0,255,0) );
    setPixel( 330, 240, RGB(0,255,255) );
    setPixel( 340, 240, RGB(0,0,255) );
    setPixel( 350, 240, RGB(255,0,255) );
    setPixel( 360, 240, RGB(255,255,255) );
}

int main(int, char **)
{
    std::cout << "Hello, world!\n";
    SDL_Window * window = SDL_CreateWindow(titulo.c_str(),
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    640, 480,
    SDL_WINDOW_RESIZABLE);

    window_surface = SDL_GetWindowSurface(window);

    pixels = (unsigned int *) window_surface->pixels;
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
                    pixels = (unsigned int *) window_surface->pixels;
                    width = window_surface->w;
                    height = window_surface->h;
                    printf("Size changed: %d, %d\n", width, height);
                }
            }

            // Se o mouse é movimentado
            if(event.type == SDL_MOUSEMOTION)
            {
                // Mostra as posições x e y do mouse
                showMousePosition(window,event.motion.x,event.motion.y);
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
				/*Se o botão esquerdo do mouse é pressionado */
                if(event.button.button == SDL_BUTTON_LEFT)
				{
					printf("Mouse pressed on (%d,%d)\n",event.motion.x,event.motion.y) ;
				}
            }
        }

        // Seta a cor de fundo da janela para a informada nas
        // constantes VERMELHO, VERDE e AZUL
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                setPixel(x, y, RGB(VERMELHO,VERDE,AZUL));
            }
        }

        display();

        SDL_UpdateWindowSurface(window);
    }

}
