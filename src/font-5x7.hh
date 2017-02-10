#pragma once

#include <SDL2/SDL.h>

class Font5x7 {
private:

  SDL_Texture *m_texture;
  SDL_Renderer *m_renderer;
  
  void render_char(SDL_Surface*, int, uint8_t*);
  
public:

  Font5x7();
  Font5x7(SDL_Renderer*);
  ~Font5x7();

  SDL_Texture *texture();

  void draw(int, int, char*);
  void drawf(int, int, char*, ...);
};
