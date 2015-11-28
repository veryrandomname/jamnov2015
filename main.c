
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include <bit_array.h>

#define NPERCEPTIONS 20
#define NSTATES 100
#define NACTIONS 20
#define WORLDSIZE 100

#define ACTION_MATE 1
#define ACTION_ATTACK 2
#define ACTION_BLOCK 3
#define ACTION_EAT 4
#define ACTION_UP 5
#define ACTION_DOWN 6
#define ACTION_LEFT 7
#define ACTION_RIGHT 8

#define PROPERTY_FOOD 8

typedef unsigned char byte;
typedef unsigned int pos;

struct sa {
  byte s; // a state
  byte a; // a action
};

typedef struct sa sa;

struct animal {
  byte s; // state of animal
  byte a; // next action animal will perfom
  sa d[NSTATES][NPERCEPTIONS]; //state translation function (States,Perception) -> (new State, Action)
};

typedef struct animal animal;

struct tile {
  animal* a1;
  animal* a2;
  byte properties;
};

typedef struct tile tile;

tile world[WORLDSIZE][WORLDSIZE]; //2d map with 2 animals at every tile

byte perceptionAt(pos x, pos y) {
  return world[x][y].properties;
}

void stepAnimal(animal a1, pos x, pos y) {
  BIT_ARRAY* b = bit_array_create(NSTATES);
  bit_array_clear_all(b);
  sa r;
  r.s = a1.s;
  r.a = 0;

  while (r.a == 0 && bit_array_get_bit(b,r.s) == 0) {
    r = a1.d[r.s][perceptionAt(x,y)];
    bit_array_set_bit(b,r.s);
  }
  a1.s = r.s;
  a1.a = r.a;
}

byte freePlace(pos x, pos y) {
  return (world[x][y].a1 == NULL || world[x][y].a2 == NULL);
}

struct sexPos {
  pos x;
  pos y;
};

typedef struct sexPos sexPos;

//return {2,*} is an error
sexPos nextFreePlace(pos x, pos y) {
  sexPos p = {0,0};

  if (freePlace(x-1,y))
    p.x = -1;
  else if (freePlace(x+1,y))
    p.x = 1;
  else if (freePlace(x,y-1))
    p.y = -1;
  else if (freePlace(x,y+1))
    p.y = 1;
  else
    p.x = 2;

  return p;
}

byte animalMoveHelper(pos x, pos y, byte whichAnimal, pos dx, pos dy) {
  if(freePlace(x+dx,y+dy)) {
    tile t = world[x][y];
    tile t2 = world[x+dx][y+dy];
    if (t2.a1 == NULL) {
      if(whichAnimal)
        t2.a1 = t.a1;
      else
        t2.a1 = t.a2;
      }
    else {
      if(whichAnimal)
        t2.a2 = t.a1;
      else
        t2.a2 = t.a2;
    }
    return 1;
  }
  else
    return 0;
}

void doActions(pos x, pos y) {
  tile t = world[x][y];
  animal* a1 = t.a1;
  animal* a2 = t.a2;

  sexPos f = nextFreePlace(x,y);

  if (a1 != NULL && a2 != NULL && (*a2).a == ACTION_MATE && (*a1).a == ACTION_MATE && f.x != 2 ) {
    animal child;//TODO = makeChild(*a1,*a2,50);
    tile t2 = world[x+f.x][y+f.y];
    if (t2.a1 == NULL)
      t2.a1 = &child;
    else
      t2.a2 = &child;
  }
  else if ((*a1).a == ACTION_ATTACK && (*a2).a != ACTION_BLOCK)
    world[x][y].a2 = NULL;
  else if ((*a2).a == ACTION_ATTACK && (*a1).a != ACTION_BLOCK)
    world[x][y].a1 = NULL;
  else if ((*a1).a == ACTION_EAT || (*a2).a == ACTION_EAT) //&& world[x][y].properties & PROPERTY_FOOD == PROPERTY_FOOD)
    world[x][y].properties = world[x][y].properties ^ PROPERTY_FOOD;
  else if ((*a1).a == ACTION_RIGHT && freePlace(x+1,y))
    animalMoveHelper(x,y,1,1,0);
  else if ((*a1).a == ACTION_LEFT && freePlace(x-1,y))
    animalMoveHelper(x,y,1,-1,0);
  else if ((*a1).a == ACTION_UP && freePlace(x,y-1))
    animalMoveHelper(x,y,1,0,-1);
  else if ((*a1).a == ACTION_DOWN && freePlace(x,y+1))
    animalMoveHelper(x,y,1,0,1);
  else if ((*a1).a == ACTION_RIGHT && freePlace(x+1,y))
    animalMoveHelper(x,y,1,1,0);
  else if ((*a2).a == ACTION_LEFT && freePlace(x-1,y))
    animalMoveHelper(x,y,2,-1,0);
  else if ((*a2).a == ACTION_UP && freePlace(x,y-1))
    animalMoveHelper(x,y,2,0,-1);
  else if ((*a2).a == ACTION_DOWN && freePlace(x,y+1))
    animalMoveHelper(x,y,2,0,1);
  else if ((*a2).a == ACTION_RIGHT && freePlace(x+1,y))
    animalMoveHelper(x,y,2,1,0);

    
}

animal makeChild(animal, animal, int);
void randolution(sa**, short);

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


