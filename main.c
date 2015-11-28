
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include <bit_array.h>
//#include "behaviour.c"

#define NPERCEPTIONS 20
#define NSTATES 255 //less states break stuff
#define NACTIONS 9
#define WORLDSIZE 100
#define NANIMALS 100000

#define ACTION_MATE 1
#define ACTION_ATTACK 2
#define ACTION_BLOCK 3
#define ACTION_EAT 4
#define ACTION_UP 5
#define ACTION_DOWN 6
#define ACTION_LEFT 7
#define ACTION_RIGHT 8

#define PROPERTY_FOOD 1

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
  byte hunger;
};

typedef struct animal animal;

struct tile {
  animal* a1;
  animal* a2;
  byte properties;
};

typedef struct tile tile;

tile world[WORLDSIZE][WORLDSIZE]; //2d map with 2 animals at every tile

static int allAnimalsSIZE = 0;
static animal allAnimals[NANIMALS];

byte perceptionAt(pos x, pos y) {
  return world[x][y].properties;
}


animal makeChild(animal, animal, int);
void randolution(sa d[NSTATES][NPERCEPTIONS], short);

void stepAnimal(animal* a1, pos x, pos y) {
  BIT_ARRAY* b = bit_array_create(NSTATES);
  bit_array_clear_all(b);
  sa r;
  r.s = (*a1).s;
  r.a = 0;

  while (r.a == 0 && bit_array_get_bit(b,r.s) == 0) {
    r = (*a1).d[r.s][perceptionAt(x,y)];
    bit_array_set_bit(b,r.s);
  }
  (*a1).s = r.s;
  (*a1).a = r.a;

  bit_array_free(b);
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
    tile* t = &world[x][y];
    tile* t2 = &world[x+dx][y+dy];
    if ((*t2).a1 == NULL) {
      if(whichAnimal){
        (*t2).a1 = (*t).a1;
        (*t).a1 = NULL;
      }
      else{
        (*t2).a1 = (*t).a2;
        (*t).a2 = NULL;
      }
    }
    else {
      if(whichAnimal){
        (*t2).a2 = (*t).a1;
        (*t).a1 = NULL;
      }
      else{
        (*t2).a2 = (*t).a2;
        (*t).a2 = NULL;
      }
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

  if(x>1 && y > 1 && x < WORLDSIZE - 1 && y < WORLDSIZE -1){
  sexPos f = nextFreePlace(x,y);
 
  if(a1 != NULL && a2 != NULL){
    if ((*a2).a == ACTION_MATE && (*a1).a == ACTION_MATE && f.x != 2 && allAnimalsSIZE < NANIMALS ) {
      //animal child = makeChild(*a1,*a2,50);
      allAnimals[allAnimalsSIZE] = makeChild(*a1,*a2,50);
      //printf("LEBEN\n");
      tile* t2 = &world[x+f.x][y+f.y];
      if ((*t2).a1 == NULL)
        (*t2).a1 = &allAnimals[allAnimalsSIZE];
      else
        (*t2).a2 = &allAnimals[allAnimalsSIZE];

      allAnimalsSIZE++;
    }
    else if ((*a1).a == ACTION_ATTACK && (*a2).a != ACTION_BLOCK){
      world[x][y].a2 = NULL;
      //printf("TOOOT\n");
      //allAnimalsSIZE--;
    }
    else if ((*a1).a == ACTION_EAT || (*a2).a == ACTION_EAT){ //&& world[x][y].properties & PROPERTY_FOOD == PROPERTY_FOOD)
      //world[x][y].properties = world[x][y].properties ^ PROPERTY_FOOD;
      if(a1 != NULL)
        (*a1).hunger += 1000;
      if(a2 != NULL)
        (*a2).hunger += 1000;
    }
    else if ((*a2).a == ACTION_ATTACK && (*a1).a != ACTION_BLOCK){
      world[x][y].a1 = NULL;
      //printf("TOOOT\n");
      //allAnimalsSIZE--;
    }
  }

  if(a1 != NULL) {
    //animalMoveHelper(x,y,1,1,0);
    //printf("%u\n",(*a1).a);
    //printf("%u\n",(*a1).a);
    if ((*a1).a == ACTION_RIGHT && freePlace(x+1,y))
      animalMoveHelper(x,y,1,1,0);
    else if ((*a1).a == ACTION_LEFT && freePlace(x-1,y))
      animalMoveHelper(x,y,1,-1,0);
    else if ((*a1).a == ACTION_UP && freePlace(x,y-1))
      animalMoveHelper(x,y,1,0,-1);
    else if ((*a1).a == ACTION_DOWN && freePlace(x,y+1))
      animalMoveHelper(x,y,1,0,1);
    else if ((*a1).a == ACTION_RIGHT && freePlace(x+1,y))
      animalMoveHelper(x,y,1,1,0);

    (*a1).a = 0;
  }
  if (a2 != NULL){
    if ((*a2).a == ACTION_LEFT && freePlace(x-1,y))
      animalMoveHelper(x,y,2,-1,0);
    else if ((*a2).a == ACTION_UP && freePlace(x,y-1))
      animalMoveHelper(x,y,2,0,-1);
    else if ((*a2).a == ACTION_DOWN && freePlace(x,y+1))
      animalMoveHelper(x,y,2,0,1);
    else if ((*a2).a == ACTION_RIGHT && freePlace(x+1,y))
      animalMoveHelper(x,y,2,1,0);

    (*a2).a = 0;
  }

  }

}

void stepWorld() {
  for (int x=0; x < WORLDSIZE; x++) {
    for (int y=0; y < WORLDSIZE; y++) {
      tile t = world[x][y];
      if(t.a1 != NULL)
        stepAnimal(t.a1,x,y);
      if(t.a2 != NULL)
        stepAnimal(t.a2,x,y);
    }
  }
  for (int x=0; x < WORLDSIZE; x++) {
    for (int y=0; y < WORLDSIZE; y++) {
      doActions(x,y);
    }
  }

  //hunger
  for (int x=0; x < WORLDSIZE; x++) {
    for (int y=0; y < WORLDSIZE; y++) {
      if(world[x][y].a1 != NULL){
        animal* a1 = world[x][y].a1;
        (*a1).hunger -= 1;
        if ((*a1).hunger == 0)
          world[x][y].a1 = NULL;
      }
      if(world[x][y].a2 != NULL){
        animal* a2 = world[x][y].a2;
        (*a2).hunger -= 1;
        if ((*a2).hunger == 0)
          world[x][y].a2 = NULL;
      }
    }
  }
}


int main(int argc, char *argv[]) {
    int posX = 100, posY = 100, width = 800, height = 600;

    SDL_Window *win = SDL_CreateWindow("Untitled", posX, posY, width, height, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    for (int i=5; i < WORLDSIZE*WORLDSIZE-100; i++) {
      allAnimalsSIZE++;
      //sa d[NSTATES][NPERCEPTIONS]; //state translation function (States,Perception) -> (new State, Action)
      animal a;
      a.s = 0;
      a.a = 0;
      a.hunger = 100;
      //a.d = malloc(sizeof(sa)*NSTATES*NPERCEPTIONS);
      allAnimals[i] = a;
      randolution(a.d,100);

      world[i%WORLDSIZE][i/WORLDSIZE].a1 = &allAnimals[i];
    }

    //food everywhere
    for (int x=0; x < WORLDSIZE; x++) {
      for (int y=0; y < WORLDSIZE; y++) {
        world[x][y].properties = 1;
      }
    }

    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

        SDL_RenderClear(renderer);
        for (int x=0; x < WORLDSIZE; x++) {
          for (int y=0; y < WORLDSIZE; y++) {
            if (world[x][y].a1!=NULL)
              SDL_RenderDrawPoint(renderer, x*5+2, y*5+2);
            if (world[x][y].a2!=NULL)
              SDL_RenderDrawPoint(renderer, x*5+3, y*5+3);
          }
        }
        SDL_RenderPresent(renderer);

        stepWorld();
        //sleep(0.1);
        //printf("hello\n");
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    return EXIT_SUCCESS;
}


void randolution(sa d[NSTATES][NPERCEPTIONS], short p){
	for(byte i = 0; i < NPERCEPTIONS; ++i){
	  for(byte j = 0; j < NSTATES; ++j){
	    int r = rand()%100;
	    if(r <= p){
	      d[j][i].s = (byte)rand()%NSTATES;
        if(rand()%100 > 70 )
          d[j][i].a = (byte)rand()%NACTIONS;
        else
          d[j][i].a = ACTION_EAT;
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
  newChild.hunger = a.hunger;
  return newChild;

}


