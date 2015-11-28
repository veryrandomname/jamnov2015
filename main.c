#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>

#define NPERCEPTIONS 20
#define NSTATES 100
#define NACTIONS 20
#define WORLDSIZE 100

typedef unsigned char byte;
typedef unsigned int pos;



struct sa {
  byte s; // a state
  byte a; // a action
};

typedef struct sa sa;

struct animal {
  pos x; // x pos of animal
  pos y; // y pos of animal
  byte s; // state of animal
  byte a; // next action animal will perfom
  sa d[NSTATES][NPERCEPTIONS]; //state translation function (States,Perception) -> (new State, Action)
};
//void randolution(sa**, short);j
typedef struct animal animal;


//typedef struct sa sa;

animal* world[WORLDSIZE][WORLDSIZE][2]; //2d map with 2 animals at every tile
animal makeChild(animal, animal, int);
void randolution(sa**, short);

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
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

void randolution(sa** d, short p){
	for(byte i = 0; i < NPERCEPTIONS; ++i){
	  for(byte j = 0; j < NSTATES; ++j){
	    int r = rand()%100;
	    if(r <= p){
	      d[j][i].s = (char)rand()%NSTATES;
	    }
	    else{
	      d[j][i].a = (char)rand()%NACTIONS;
	    }
	  }	
	}
}
animal makeChild(animal a, animal b, int p){
	animal newChild; 
	for(int i = 0; i < NSTATES; ++i){
	  for(int j = 0; j < NPERCEPTIONS; ++j){
	    int r = rand()%100;
	    if(r <= p)
	      newChild.d[i][j] = a.d[i][j];	  
	    else 
	      newChild.d[i][j] = b.d[i][j]; 
	  }
  }
  newChild.a = 0;
  newChild.s = a.s;
  return newChild;

}

