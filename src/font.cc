#include "font.hh"

#include <iostream>
using namespace std;

/* image class */
Image::Image() {
  m_width = 0;
  m_height = 0;
}

Image::Image(int w, int h) {
  resize(w, h);
}

void Image::resize(int w, int h) {
  m_width = w;
  m_height = h;
  m_pixels.resize(w * h);
}

void Image::clear() {
  for(auto &p : m_pixels) p = 0;
}

int Image::width() {
  return m_width;
}

int Image::height() {
  return m_height;
}

vector<uint8_t> & Image::pixels() {
  return m_pixels;
}

void Image::set_pixels(int w, int h, uint8_t *source) {
  resize(w, h);

  for(auto &p : m_pixels) {
    p = *source;
    source++;
  } 
}

void Image::blit(Image &source, int to_x, int to_y) {

  auto dest_pixels = m_pixels.begin() + (to_y * m_width) + to_x;
  auto src_pixels = source.pixels().begin();
  
  for(int y = 0; y < source.height(); y++) {
    for(int x = 0; x < source.width(); x++) {

      *dest_pixels = *src_pixels;

      dest_pixels++;
      src_pixels++;          
    }
    
    to_y++;
    if(to_y >= m_height) return;

    dest_pixels += m_width - source.width();
  }
}

SDL_Texture * Image::make_texture(SDL_Renderer *rndr) {

  SDL_Surface *surface = NULL;
  surface = SDL_CreateRGBSurface(0, m_width, m_height, 32, 0, 0, 0, 0);
  if(!surface) throw "Could not create surface";

  //cout << "surface width=" << surface->w << "  height=" << surface->h << endl;
  //cout << "  pixels.size=" << char_palette.pixels().size() << endl;
  
  uint8_t *write_pixels = (uint8_t *)surface->pixels;
  for(auto &src_pixel : m_pixels) {
    uint8_t value = src_pixel;
  
    *write_pixels = value; write_pixels++;
    *write_pixels = value; write_pixels++;
    *write_pixels = value; write_pixels++;
    *write_pixels = value; write_pixels++;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(rndr, surface);
  
  SDL_FreeSurface(surface);

  return texture;
}


/* glyph */

Glyph::Glyph() { }

Glyph::Glyph(FT_Face face, int char_num) {
  
  FT_UInt glyph_index = FT_Get_Char_Index( face, char_num );

  FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );

  FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

  m_advance = face->glyph->advance.x >> 6;
  m_yoffset = face->glyph->metrics.height >> 6;
  
  int width  = face->glyph->bitmap.width;
  int height = face->glyph->bitmap.rows;
  
  //m_yoffset = face_size - some_height; //face->glyph->bitmap_top;
  //m_yoffset = face->glyph->bitmap_top;

  m_image.set_pixels(width, height, face->glyph->bitmap.buffer);
}

Image & Glyph::image() {
  return m_image;
}

int Glyph::yoffset() {
  return m_yoffset;
}

/* font API */

Font::Font() {
  m_texture_width  = 0;
  m_texture_height = 0;
  m_char_width     = 0;
  m_char_height    = 0;
  m_texture        = NULL;
  m_renderer       = NULL;
}

Font::Font(SDL_Renderer *r) {
  Font();
  m_renderer = r;
}

void Font::set_glyphs(vector<Glyph> &glyphs) {

  m_char_width  = 0;
  m_char_height = 0;

  for(auto &glyph : glyphs) {
    Image &img(glyph.image());

    if(img.width()  > m_char_width)  m_char_width  = img.width();
    if(img.height() > m_char_height) m_char_height = img.height();
  }

  Image char_palette(m_char_width*16, m_char_height*16);
  //  cout << "char_palette width=" << char_palette.width()
  //    << "  height=" << char_palette.height() << endl;
  
  int draw_x = 0;
  int draw_y = 0; //m_char_height; //-5;

  for(auto &glyph : glyphs) {
    //char_palette.blit(glyph.image(), draw_x, draw_y + glyph.yoffset());
    int ypos = draw_y + (m_char_height - glyph.yoffset());
    char_palette.blit(glyph.image(), draw_x, ypos);

    draw_x += m_char_width;
    if(draw_x >= m_char_width*16) {
      draw_x = 0;
      draw_y += m_char_height;
    }
  }

  m_texture = char_palette.make_texture(m_renderer);
  m_texture_width = char_palette.width();
  m_texture_height = char_palette.height();  

}

void Font::draw( int x, int y, const std::string &str ) {
  draw(x, y, str.c_str());
}

void Font::draw( int x, int y, const char *text ) {

  SDL_Rect dst = { x, y, m_char_width, m_char_height };
  SDL_Rect src = { 0, 0, m_char_width, m_char_height };

  while(*text) {

    src.x = (*text & 15) * m_char_width;
    src.y = (*text >> 4) * m_char_height;
    
    SDL_RenderCopy(m_renderer, m_texture, &src, &dst);

    dst.x += m_char_width;
    text++;
  }
}

void Font::draw(int x, int y) {
  SDL_Rect dst = { x, y, m_texture_width, m_texture_height };
  SDL_RenderCopy(m_renderer, m_texture, NULL, &dst);
}

void Font::draw_bb(int x, int y) {
  
  SDL_Rect dst = { x, y, m_texture_width+1, m_texture_height+1 };
  SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
  
  SDL_RenderDrawRect(m_renderer, &dst);
}

int Font::char_width()  { return m_char_width; }
int Font::char_height() { return m_char_height; }

int Font::pal_width()  { return m_texture_width; }
int Font::pal_height() { return m_texture_height; }


bool Font::is_rendered() {
  return m_texture != NULL;
}


/* font renderer */

FontRenderer::FontRenderer() {
  m_library  = NULL;
  m_face     = NULL;
  m_renderer = NULL;
}

FontRenderer::FontRenderer( SDL_Renderer *rndr, const char *filename ) {

  m_renderer = rndr;
  
  if(FT_Init_FreeType( &m_library ))
    throw "FT_Init_FreeType failed";

  if(FT_New_Face(m_library, filename, 0, &m_face))
    throw "FT_New_Face failed";
}

Font FontRenderer::make_font(int size) {

  FT_Set_Pixel_Sizes(m_face, 0, size);

  Font new_font(m_renderer);

  if(m_face) {
    std::vector<Glyph> glyphs(256);
  
    for(int i = 0; i < 256; i++)
      glyphs[i] = Glyph(m_face, i);

    new_font.set_glyphs(glyphs);
  }
  
  return new_font;
}
