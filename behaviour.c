#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned int pos;

struct worldstate{
	char water;
	char animal;
	char food;
};

typedef struct worldstate ws;



ws Perception(byte p, ws ws1){ //different 8-bit perception states

	byte o0 = 4; // o0 = other animal and is 
	byte w = 3; // w0 = water level determination
	byte f0 = 8; // f0 = is food availibe

	if ((p & o0)){
		ws1.animal = 1;
	}
	else {
		ws1.animal = 0;
	}

	ws1.water = (p & w);

	if ((p & f0)){
		ws1.food = 1;
	}
	else {
		ws1.food = 0;
	}
	return ws1;	
}

create_background(int x, int y){

	SDL_Surface* screen = NULL;
	SDL_Init( SDL_INIT_EVERYTHING );
	int videoW = 1280;
	int videoH = 720;
	int hori = videoW / x;
	int verti = videoH / y;
	screen = SDL_SetVideoMode( videoW, videoH, 32, SDL_SWSURFACE );

	for (int i = 0; i < x; ++i){
		int SDL_RenderDrawLine(SDL_Renderer* renderer,
           hori * i,
           0,
           hori * i,
           videoH)
	}

	for (int j = 0; j < y; ++j){
		int SDL_RenderDrawLine(SDL_Renderer* renderer,
	       0,
	       verti * j,
	       videoW
	       verti * j)
	}
	//Update Screen 
	SDL_Flip( screen );

	//Pause 
	SDL_Delay( 2000 );

}