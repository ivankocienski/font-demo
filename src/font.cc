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

//uint8_t Image::read(int x, int y) {
//  if(x < 0 || x >= m_width) return 0;
//  if(y < 0 || y >= m_height) return 0;

//  return m_pixels[y * m_height + x];
//}

//void Image::write(int x, int y, uint8_t c) {
//  if(x < 0 || x >= m_width)  return;
//  if(y < 0 || y >= m_height) return;

//  m_pixels[y * m_height + x] = c;
//}

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

Glyph::Glyph() {
}

Glyph::Glyph(FT_Face face, int face_size, int char_num) {
  
  FT_UInt glyph_index = FT_Get_Char_Index( face, char_num );

  FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );

  FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

  m_advance = face->glyph->advance.x >> 6;

  int width  = face->glyph->bitmap.width;
  int height = face->glyph->bitmap.rows;
  
  m_yoffset = face_size - face->glyph->bitmap_top;

  m_image.set_pixels(width, height, face->glyph->bitmap.buffer);
}

Image & Glyph::image() {
  return m_image;
}

int Glyph::yoffset() {
  return m_yoffset;
}

/* font renderer */

FontRenderer::FontRenderer() {
  m_library = NULL;
  m_face = NULL;
  m_size = 0;
}

FontRenderer::FontRenderer( int size, const char *filename ) {

  m_size = size;

  if(FT_Init_FreeType( &m_library ))
    throw "FT_Init_FreeType failed";

  if(FT_New_Face(m_library, filename, 0, &m_face))
    throw "FT_New_Face failed";

  FT_Set_Pixel_Sizes(m_face, 0, size);
}

void FontRenderer::render() {

  m_glyphs.resize(256);

  for(int i = 0; i < 256; i++)
    m_glyphs[i] = Glyph(m_face, m_size, i);
}

void FontRenderer::max_size(int &max_width, int &max_height) {
  max_width = 0;
  max_height = 0;

  for(auto &glyph : m_glyphs) {
    Image &img(glyph.image());

    if(img.width() > max_width) max_width = img.width();
    if(img.height() > max_height) max_height = img.height();
  }
}

std::vector<Glyph> &FontRenderer::glyphs() {
  return m_glyphs;
}


/* main font API */

Font::Font() {
}

void Font::initialize( SDL_Renderer *out_rndr, int size, const char *path ) {

  FontRenderer renderer(size, path);
  renderer.render();

  int max_width = 0;
  int max_height = 0;
  renderer.max_size(max_width, max_height);
 
  cout << "max width=" << max_width << "  height=" << max_height << endl;

  Image char_palette(max_width*16, (max_height+1)*16);
  cout << "char_palette width=" << char_palette.width() << "  height=" << char_palette.height() << endl;
  
  int draw_x = 0;
  int draw_y = 0;

  cout << "blitting " << renderer.glyphs().size() << " glyphs" << endl;
  
  for(auto &glyph : renderer.glyphs()) {
    char_palette.blit(glyph.image(), draw_x, draw_y + glyph.yoffset());

    draw_x += max_width;
    if(draw_x >= max_width*16) {
      draw_x = 0;
      draw_y += max_height;
    }
  }

  m_texture = char_palette.make_texture(out_rndr);
  m_texture_width = char_palette.width();
  m_texture_height = char_palette.height();  
}

void Font::draw( int x, int y, const char *text ) {

}

SDL_Texture *Font::texture() {
  return m_texture;
}

void Font::draw(SDL_Renderer *rndr, int x, int y) {
  
  SDL_Rect dst = { x, y, m_texture_width, m_texture_height };

  SDL_RenderCopy(rndr, m_texture, NULL, &dst);
}

void Font::draw_bb(SDL_Renderer *rndr, int x, int y) {
  
  SDL_Rect dst = { x, y, m_texture_width+1, m_texture_height+1 };
  SDL_SetRenderDrawColor(rndr, 255, 0, 255, 255);
  
  SDL_RenderDrawRect(rndr, &dst);
}

