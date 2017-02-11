#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <cstring>

using namespace std;

#include "font.hh"
#include "font-5x7.hh"

#include <SDL2/SDL.h>

// SDL
static SDL_Window   *main_window = NULL;
static SDL_Renderer *rndr = NULL;

// state
static vector<string> text_strings;

// config from argv
static int start_size = 40;
static string font_path;
static string strings_path;


static void load_strings() {

  if(strings_path.empty()) {
    text_strings.resize(3);
    text_strings[0] = "AaBbCc123[]()*~?";
    text_strings[1] = "Hello, World";
    text_strings[2] = "The quick fox jumped over the lazy dog";
    return;
  }
  
  fstream file;

  file.open(strings_path, ios_base::in);

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
    
  debug.drawf(xpos, ypos, "Font path: %s", font_path.c_str());
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
  
  debug.drawf(xpos, ypos, "Font size=%d  path=%s", font_size, font_path.c_str());
  ypos += 20;
    
  for(auto & str : text_strings) {
    font.draw(xpos, ypos, str);
    
    ypos += font.char_height() + 10;
  }
}

static void print_help() {

  cout << "help" << endl
       << "  font-demo [--strings PATH] [--size SIZE] [-h] TTF_PATH" << endl
       << "    --strings PATH   path to use for loading specific strings" << endl
       << "    --size SIZE  use SIZE as a starting size. Must be between 10-90 integer" << endl
       << "    -h show this help" << endl
       << endl
       << endl;

  // TODO: basic usage here
}

static void process_args(int argc, char ** argv) {

  while(true) {
    argc--;
    argv++;

    if(!argc) break;
    
    if(!strcmp(*argv, "-h")) {
      print_help();
      exit(0);
    }

    if(!strcmp(*argv, "--strings")) {
      if(!argc) {
	cerr << "error: missing argument" << endl;
	print_help();
	exit(-1);
      }

      argc--;
      argv++;

      if(**argv == '-') {
	cerr << "error: bad argument" << endl;
	print_help();
	exit(-1);
      }
      
      strings_path = *argv;
      continue;
    }

    if(!strcmp(*argv, "--size")) {
      if(!argc) {
	cerr << "error: missing argument" << endl;
	print_help();
	exit(-1);
      }

      argc--;
      argv++;
      
      if(**argv == '-') {
	cerr << "error: bad argument" << endl;
	print_help();
	exit(-1);
      }

      start_size = atoi(*argv);
      continue;
    }

    font_path = *argv;
    break;
  }

  cout << "strings_path=" << strings_path << endl;
  cout << "start_size=" << start_size << endl;
  cout << "font_path=" << font_path << endl;

  if(strings_path.empty()) {
    cout << "[no strings defined, using defaults]" << endl;
  }

  if(start_size < 10 || start_size > 90) {
    cerr << "error: start_size (" << start_size << ") out of range, must be between 20-90" << endl;
    exit(-1);
  }

  if(font_path.empty()) {
    cerr << "error: missing font file to load" << endl;
    print_help();
    exit(-1);
  }
}

int main(int argc, char ** argv) { 

  process_args(argc, argv);
  
  int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
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

  vector<Font> fonts(100);
  int current_font = start_size;

  FontRenderer renderer(rndr, font_path.c_str());
  fonts[current_font] = renderer.make_font(current_font);

  load_strings();
  
  Font5x7 debug(rndr);
  
  //font.initialize(rndr, font_size, font_path);
  bool run_app = true;
  bool redraw = true;
  bool string_mode = false;
  
  while(run_app) {
    
    SDL_Delay(40);

    SDL_Event event;

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
	  if(current_font > 10) {
	    current_font--;
	    if(!fonts[current_font].is_rendered())
	      fonts[current_font] = renderer.make_font(current_font);
	    redraw = true;	    
	  }
	  break;
	
	case SDLK_PAGEDOWN:
	  if(current_font < 90) {
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
