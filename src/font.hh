#pragma once

#include <SDL2/SDL.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <cstdint>

/* NOTE: at the moment only ONE font is handled. boo! */

class Image {
private:

  std::vector<uint8_t> m_pixels;
  int m_width;
  int m_height;

public:

  Image();
  Image(int, int);

  void resize(int, int);
  void clear();
  int width();
  int height();

  //  uint8_t read(int, int);
  //  void write(int, int, uint8_t);
  void set_pixels(int, int, uint8_t*);
  void blit(Image&, int, int);

  std::vector<uint8_t> & pixels();

  SDL_Texture * make_texture(SDL_Renderer*);
};

class Glyph {
private:

  Image m_image;
  int m_advance;
  int m_yoffset;
  
public:
  Glyph();
  Glyph(FT_Face, int, int);

  Image &image();
  int yoffset();
  
};

class FontRenderer {
private:

  FT_Library m_library;
  FT_Face m_face;

  int m_size;

  std::vector<Glyph> m_glyphs;

public:

  FontRenderer();
  FontRenderer( int, const char* );

  void render();
  void max_size(int&, int&);

  std::vector<Glyph> &glyphs();
};

class Font {
private:

  FontRenderer m_renderer;

  SDL_Texture *m_texture;
  int m_texture_width;
  int m_texture_height;
  
public:
  
  Font();

  void initialize( SDL_Renderer*, int, const char* );
  void draw( int, int, const char* );

  SDL_Texture* texture();
  void draw(SDL_Renderer*, int, int);
  void draw_bb(SDL_Renderer*, int, int);
};

