
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include <bit_array.h>
//#include "behaviour.c"

#define NPROPERTIES 2
#define NSTATES 15
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
  byte r;// = (unsigned char)rand();  
  byte g;// = (unsigned char)rand();  
  byte b;// = (unsigned char)rand();
  sa d[NSTATES][NPROPERTIES]; //state translation function (States,Perception) -> (new State, Action)
  byte hunger;
  byte valid;
};

typedef struct animal animal;

struct tile {
  animal* a1;
  animal* a2;
  byte properties;
};

typedef struct tile tile;

tile world[WORLDSIZE][WORLDSIZE]; //2d map with 2 animals at every tile

static int allAnimalsI = 0;
static int allAnimalsN = 0;
static animal allAnimals[NANIMALS];

byte perceptionAt(pos x, pos y) {
  return world[x][y].properties;
}

animal makeChild(animal*, animal*, int, int);
void randolution(sa d[NSTATES][NPROPERTIES], short);

static BIT_ARRAY* stepAnimalBitArray;
void stepAnimal(animal* a1, pos x, pos y) {
  bit_array_clear_all(stepAnimalBitArray);
  sa r;
  r.s = a1->s;
  r.a = 0;

  //DEBUGINFO
  sa r2;
  r2 = r;
  //r2.s = 9;

  while (r.a == 0 && bit_array_get_bit(stepAnimalBitArray,r.s) == 0) {
    r = a1->d[r.s][perceptionAt(x,y)];
    if(r.s >= NSTATES){
      printf("State %u from animal %u\n",r.s,a1->r);
      printf("state+1: %u\n",r.s+1);
      printf("Previus state %u \n",r2.s);
      printf("Perception %u \n",perceptionAt(x,y));
      r.s=1;
    }
    r2 = r;
    bit_array_set_bit(stepAnimalBitArray,r.s);
  }
  a1->s = r.s;
  a1->a = r.a;
}

byte freePlace(pos x, pos y) {
  return ((x < WORLDSIZE) && (y < WORLDSIZE) && (world[x][y].a1 == NULL || world[x][y].a2 == NULL));
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
  //if((x!=0||dx==1) && (y!=0||dy==1) && (x!=WORLDSIZE||dx==-1) && (y!=WORLDSIZE||dy==-1) && freePlace(x+dx,y+dy)) {
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

void killAnimal1(pos x,pos y) {
  world[x][y].a1->valid = 0;
  world[x][y].a1 = NULL;
  allAnimalsN--;
  //printf("KILL\n");
}

void killAnimal2(pos x,pos y) {
  world[x][y].a2->valid = 0;
  world[x][y].a2 = NULL;
  allAnimalsN--;
  //printf("KILL\n");
}

int invalidAnimal() {
  int i = allAnimalsI;

  while (1){
    if(!allAnimals[i].valid){
      allAnimalsI = i+1;
      if(allAnimalsI == NANIMALS)
        allAnimalsI = 0;
      return i;
    }
    i++;
    if(i == NANIMALS)
      i = 0;
    if(i==allAnimalsI)
      break;
  }
  return -1;
}

void doActions(pos x, pos y) {
  tile t = world[x][y];
  animal* a1 = t.a1;
  animal* a2 = t.a2;

  sexPos f = nextFreePlace(x,y);
 
  if(a1 != NULL && a2 != NULL){
    if ((*a2).a == ACTION_MATE && (*a1).a == ACTION_MATE && f.x != 2 && allAnimalsN < NANIMALS) {
      int i = invalidAnimal();
      if (i != -1) {
        //animal child = makeChild(*a1,*a2,50);
        allAnimals[i] = makeChild(a1,a2,50, 90);
        //printf("LEBEN\n");
        tile* t2 = &world[x+f.x][y+f.y];
        if ((*t2).a1 == NULL)
          (*t2).a1 = &allAnimals[i];
        else
          (*t2).a2 = &allAnimals[i];
      }
    }
    else if ((*a1).a == ACTION_ATTACK && (*a2).a != ACTION_BLOCK){
      //a1->hunger = (a2->hunger/2) | a1->hunger;
      killAnimal2(x,y);
      a1->hunger = 255;
      //world[x][y].a2 = NULL;
      //printf("TOOOT\n");
      //allAnimalsSIZE--;
    }
    else if ((*a2).a == ACTION_ATTACK && (*a1).a != ACTION_BLOCK){
      //a2->hunger = a2->hunger | (a1->hunger/2);
      killAnimal1(x,y);
      a2->hunger = 255;
      //world[x][y].a1 = NULL;
      //printf("TOOOT\n");
      //allAnimalsSIZE--;
    }
  }

  if(a1 != NULL) {
    //animalMoveHelper(x,y,1,1,0);
    //printf("%u\n",(*a1).a);
    //printf("%u\n",(*a1).a);
    if (a1->a == ACTION_EAT && ((world[x][y].properties & PROPERTY_FOOD) == PROPERTY_FOOD)){
      a1->hunger = 255;
      world[x][y].properties = world[x][y].properties ^ PROPERTY_FOOD;
    }
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
    if (a2->a == ACTION_EAT && ((world[x][y].properties & PROPERTY_FOOD) == PROPERTY_FOOD)){
      a2->hunger = 55;
      world[x][y].properties = world[x][y].properties ^ PROPERTY_FOOD;
    }
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

void populate();

static int stepTimer = 8000;
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
      if(rand()%1000 > stepTimer)
        world[x][y].properties = world[x][y].properties | PROPERTY_FOOD;
      /*
      float f = (float)stepTimer;
      if(45+(sin(sqrt(f)*f/55000.0))*40 < x && x < 55+(sin(sqrt(f)*f/55000.0)*40) )
        world[x][y].properties = world[x][y].properties | PROPERTY_FOOD;
        */
      //printf("rand %u and stepTimer %u\n",rand()%10000,stepTimer);
    }
  }

  //hunger
  for (int x=0; x < WORLDSIZE; x++) {
    for (int y=0; y < WORLDSIZE; y++) {
      if(world[x][y].a1 != NULL){
        animal* a1 = world[x][y].a1;
        if (a1->hunger == 0)
          killAnimal1(x,y);
        else
          a1->hunger -= 1;
      }
      if(world[x][y].a2 != NULL){
        animal* a2 = world[x][y].a2;
        if (a2->hunger == 0)
          killAnimal2(x,y);
        else
          a2->hunger -= 1;
      }
    }
  }
  if(stepTimer < 980)
    stepTimer++;

  //printf("%u\n",allAnimalsN);

  if(allAnimalsN < 50){
    //killall
    for (int x=0; x < WORLDSIZE; x++) {
      for (int y=0; y < WORLDSIZE; y++) {
        animal* a1 = world[x][y].a1;
        animal* a2 = world[x][y].a2;
        if(a1)
          killAnimal1(x,y);
        if(a2)
          killAnimal2(x,y);
        world[x][y].a1 = NULL;
        world[x][y].a2 = NULL;
      }
    }
    if(allAnimalsN != 0)
      printf("Error there still are %u animals\n",allAnimalsN);
    populate();
    stepTimer=0;
  }
}

void populate() {
  for (int i=5; i < WORLDSIZE*WORLDSIZE-100; i++) {
    allAnimalsN++;
    //sa d[NSTATES][NPROPERTIES]; //state translation function (States,Perception) -> (new State, Action)
    animal a;
    a.s = 0;
    a.a = 0;
    a.hunger = 100;
    a.r = (byte)rand();
    a.b = (byte)rand();
    a.g = (byte)rand();
    a.valid = 1;
    //a.d = malloc(sizeof(sa)*NSTATES*NPROPERTIES);
    allAnimals[i] = a;
    randolution(a.d,100);

    world[i%WORLDSIZE][i/WORLDSIZE].a1 = &allAnimals[i];
  }


}

int main(int argc, char *argv[]) {
  stepAnimalBitArray = bit_array_create(NSTATES);

  int posX = 100, posY = 100, width = 800, height = 800;

  SDL_Window *win = SDL_CreateWindow("Bacteria", posX, posY, width, height, 0);

  SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  populate();

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
          /*
          if ((world[x][y].properties & PROPERTY_FOOD) == PROPERTY_FOOD){
            SDL_SetRenderDrawColor(renderer, 55,55,55, 0); 
            SDL_RenderDrawPoint(renderer, x*5+1, y*5+1);
          }
          */
          if (world[x][y].a1!=NULL){
            SDL_SetRenderDrawColor(renderer, (*world[x][y].a1).r ,(*world[x][y].a1).g ,(*world[x][y].a1).b , 0);
            SDL_RenderDrawPoint(renderer, x*8+3, y*8+3);
          }
          if (world[x][y].a2!=NULL){
            SDL_SetRenderDrawColor(renderer, (*world[x][y].a2).r ,(*world[x][y].a2).g,(*world[x][y].a2).b, 0); 
            SDL_RenderDrawPoint(renderer, x*8+5, y*8+5);
          }
        }
      }
          SDL_SetRenderDrawColor(renderer, 0,0,0, 0); 
      SDL_RenderPresent(renderer);

      stepWorld();
      //sleep(0.1);
      //printf("hello\n");
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);

  bit_array_free(stepAnimalBitArray);

  return EXIT_SUCCESS;
}


void randolution(sa d[NSTATES][NPROPERTIES], short p){
  for(byte i = 0; i < NSTATES; i++){
    for(byte j = 0; j < NPROPERTIES; j++){
	    int r = rand()%100;
	    if(r <= p){
	      d[i][j].s = ((byte)rand())%NSTATES;
        d[i][j].a = ((byte)rand())%NACTIONS;

        if(d[i][j].a == ACTION_ATTACK)
          d[i][j].a = ACTION_MATE;
        //DEBUG
        if(d[i][j].s >= NSTATES)
          printf("MAYDAYRANDOLUTIONONE\n");
        }
      //DEBUG
      if(d[i][j].s >= NSTATES)
        printf("MAYDAYRANDOLUTIONTWO\n");
	  }	
	}
}
animal makeChild(animal* a, animal* b, int p, int q){
  allAnimalsN++;

	animal newChild; 
	for(int i = 0; i < NSTATES; i++){
	  for(int j = 0; j < NPROPERTIES; j++){
	    int r = rand()%100;
	    int r2 = rand()%100;
      if(r2 <= q) {
        if(r <= p)
          newChild.d[i][j] = a->d[i][j];	  
        else 
          newChild.d[i][j] = b->d[i][j]; 
      }
      else {
        newChild.d[i][j].s = ((byte)rand())%NSTATES;
        newChild.d[i][j].a = ((byte)rand())%NACTIONS;
      }

      //DEBUG
      if(newChild.d[i][j].s >= NSTATES)
        printf("MAYDAYCHILD\n");
	  }
  }

  a->hunger = a->hunger/2;

  newChild.a = 0;
  newChild.s = a->s;
  newChild.hunger = a->hunger;
  newChild.r = (a->r/2 + b->r/2);
  newChild.g = (a->g/2 + b->g/2);
  newChild.b = (a->b/2 + b->b/2);
  newChild.valid = 1;
  return newChild;

}


