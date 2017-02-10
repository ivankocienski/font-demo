#include <iostream>
using namespace std;

#include "font.hh"
#include "font-5x7.hh"

#include <SDL2/SDL.h>

static SDL_Window   *main_window = NULL;
static SDL_Renderer *rndr = NULL;

Font font;

int main(int argc, char ** argv) { 
  SDL_Event event;
  int ret;
  bool run_app = true;

  ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  if(ret == -1) {
    fprintf(stderr, "error: failed to init SDL!\n");
    return -1;
  }

  main_window = SDL_CreateWindow(
      "SDL Window",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      800,
      600,
      SDL_WINDOW_SHOWN);
  if(!main_window) {
    fprintf(stderr, "error: failed to create main window\n");
    return -1;
  }

  rndr = SDL_CreateRenderer(main_window, -1, 0);

  Font5x7 debug(rndr);
  
  font.initialize(rndr, 40, "data/large.ttf");

  while(run_app) {

    while(SDL_PollEvent(&event)) {

      switch(event.type) {
        case SDL_QUIT:
          run_app = false;
          break; 
      } 
    }

    SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
    SDL_RenderClear(rndr);

    //...
    //font.draw(rndr, 10, 10);
    debug.draw(10, 10, "Hello, World? AaBbCc 123 ()?=~!"); 
    
    SDL_RenderPresent(rndr);
    SDL_Delay(40);
  } 

  return 0;
}
