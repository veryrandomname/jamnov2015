#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int posX = 100, posY = 100, width = 800, height = 600;

    SDL_Window *win = SDL_CreateWindow("Untitled", posX, posY, width, height, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderDrawPoint(renderer, 50, 50);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    return EXIT_SUCCESS;
}
