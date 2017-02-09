#include <iostream>
using namespace std;

#include <SDL2/SDL.h>

static SDL_Window   *main_window = NULL;
static SDL_Renderer *rndr = NULL;

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
    
    SDL_RenderPresent(rndr);
    SDL_Delay(40);
  } 

  return 0;
}
