
#include "font-5x7.hh"
#include "font-5x7-bits.hh"

Font5x7::Font5x7() {
  m_texture = NULL;
}

Font5x7::Font5x7(SDL_Renderer *rndr) {
  Font5x7();
  
  SDL_Surface *sfc = SDL_CreateRGBSurface(0, 128 * 5, 7, 32, 0, 0, 0, 0);
  if(!sfc) throw "Surface create failed";

  int char_xpos = 32*5;
  for(int char_num = 32; char_num < 128; char_num++) {
    uint8_t *char_bits = font_5x7_bits + (char_num-32) * 5;
    render_char(sfc, char_xpos, char_bits);
    char_xpos += 5;
  }

  m_texture = SDL_CreateTextureFromSurface(rndr, sfc);
  m_renderer = rndr;
  
  SDL_FreeSurface(sfc);
}

Font5x7::~Font5x7() {
  if(m_texture && SDL_WasInit(SDL_INIT_EVENTS))
    SDL_DestroyTexture(m_texture);
}

void Font5x7::render_char(SDL_Surface *sfc, int xpos, uint8_t *char_bits) {

  for(int line = 0; line < 5; line++) {
    
    uint8_t *pixels = (uint8_t *)sfc->pixels + xpos*4;
    uint8_t line_bits = *char_bits;
    
    for(int row = 0; row < 7; row++) {
      if(line_bits & 1) {
	*(pixels+0) = 255;
	*(pixels+1) = 255;
	*(pixels+2) = 255;
	*(pixels+3) = 255;
      }
	
      pixels += sfc->pitch;
      line_bits >>= 1;
    }

    char_bits++;
    xpos++;
  }
}


SDL_Texture *Font5x7::texture() {
  return m_texture;
}

void Font5x7::draw(int x, int y, char *str) {
  SDL_Rect dst = { x, y, 5, 7 };
  SDL_Rect src = { 0, 0, 5, 7 };

  while(*str) {

    src.x = *str * 5;
    SDL_RenderCopy(m_renderer, m_texture, &src, &dst);
    
    str++;
    dst.x += 6;
  }
}
