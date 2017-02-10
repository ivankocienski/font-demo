#include <iostream>
#include <vector>
#include <list>
#include <fstream>

using namespace std;

#include "font.hh"
#include "font-5x7.hh"

#include <SDL2/SDL.h>

static SDL_Window   *main_window = NULL;
static SDL_Renderer *rndr = NULL;

static const int font_size = 40;
static const char *font_path = "data/large.ttf";

vector<string> text_strings;

void load_strings() {

  fstream file;

  file.open("data/strings.txt", ios_base::in);

  if(file.is_open()) {

    list<string> read_strings;

    while(!file.eof()) {
      string line;
      getline(file, line);

      read_strings.push_back(line);
    }

    cout << "Read " << read_strings.size() << " strings" << endl;

    text_strings.assign(read_strings.begin(), read_strings.end());
  }
}

static void draw_char_palette(Font5x7 &debug, Font &font, int font_size) {

  font.draw(1, 1);
  font.draw_bb(0, 0);

  int xpos = font.pal_width() + 5;
  int ypos = 10;
    
  debug.drawf(xpos, ypos, "Font size: %d", font_size);
  ypos += 10;
    
  debug.drawf(xpos, ypos, "Font path: %s", font_path);
  ypos += 10;
    
  debug.drawf(xpos, ypos, "Palette width  : %d", font.pal_width());
  ypos += 10;

  debug.drawf(xpos, ypos, "Palette height : %d", font.pal_height());
  ypos += 10;

  debug.drawf(xpos, ypos, "Char width  : %d", font.char_width());
  ypos += 10;
    
  debug.drawf(xpos, ypos, "Char height : %d", font.char_height());
  ypos += 10;
    
}

static void draw_strings(Font5x7 &debug, Font &font, int font_size) {

  int xpos = 10;
  int ypos = 10;
  
  debug.drawf(xpos, ypos, "Font size=%d  path=%s", font_size, font_path);
  ypos += 10;
    
  for(auto & str : text_strings) {
    font.draw(xpos, ypos, str);
    
    ypos += font.char_height() + 10;
  }
}

int main(int argc, char ** argv) { 
  SDL_Event event;
  int ret;

  vector<Font> fonts(100);
  int current_font = font_size;
  
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
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if(!main_window) {
    fprintf(stderr, "error: failed to create main window\n");
    return -1;
  }

  rndr = SDL_CreateRenderer(main_window, -1, 0);

  FontRenderer renderer(rndr, font_path);
  fonts[current_font] = renderer.make_font(font_size);

  load_strings();
  
  Font5x7 debug(rndr);
  
  //font.initialize(rndr, font_size, font_path);
  bool run_app = true;
  bool redraw = true;
  bool string_mode = false;
  
  while(run_app) {
    
    SDL_Delay(40);

    while(SDL_PollEvent(&event)) {

      switch(event.type) {
      case SDL_QUIT:
	run_app = false;
	break;
	
      case SDL_WINDOWEVENT:

	switch(event.window.event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	case SDL_WINDOWEVENT_RESIZED:
	  redraw = true;
	  break;
	}
	break;
	
      case SDL_KEYDOWN:

	switch(event.key.keysym.sym) {
	case SDLK_TAB:
	  string_mode = !string_mode;
	  redraw = true;
	  break;
	  
	case SDLK_PAGEUP:
	  if(current_font > 0) {
	    current_font--;
	    if(!fonts[current_font].is_rendered())
	      fonts[current_font] = renderer.make_font(current_font);
	    redraw = true;	    
	  }
	  break;
	
	case SDLK_PAGEDOWN:
	  if(current_font < 99) {
	    current_font++;
	    if(!fonts[current_font].is_rendered())
	      fonts[current_font] = renderer.make_font(current_font);
	    redraw = true;
	  }
	  break;
	}
	break;

      } 
    }

    if(!redraw) continue;
    redraw = false;
    
    SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
    SDL_RenderClear(rndr);

    if(string_mode)
      draw_strings(debug, fonts[current_font], current_font);
    else
      draw_char_palette(debug, fonts[current_font], current_font);
    
    SDL_RenderPresent(rndr);
  } 

  return 0;
}
