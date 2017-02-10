#pragma once

#include <SDL2/SDL.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <cstdint>
#include <string>


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

class Font {
private:

  SDL_Renderer *m_renderer;
  SDL_Texture *m_texture;
  int m_texture_width;
  int m_texture_height;
  int m_char_width;
  int m_char_height;
  
public:
  
  Font();
  Font(SDL_Renderer*);

  bool is_rendered();
  void set_glyphs(std::vector<Glyph> &);
  
  //void initialize( SDL_Renderer*, int, const char* );
  void draw( int, int, const std::string& );
  void draw( int, int, const char* );

  void draw(int, int);
  void draw_bb(int, int);

  int char_width();
  int char_height();
  int pal_width();
  int pal_height();
  //char *font_name();
};


class FontRenderer {
private:

  SDL_Renderer *m_renderer;
  FT_Library m_library;
  FT_Face m_face;

public:

  FontRenderer();
  FontRenderer( SDL_Renderer*, const char* );

  void render();
  void max_size(int&, int&);

  std::vector<Glyph> &glyphs();

  Font make_font(int);
};
