#define SCALE_DOWN_FACTOR 10
#include "vc.c"

#include "./assets/nikita.c"

#define WIDTH 960
#define HEIGHT 720

float sinf(float);

uint32_t dst[WIDTH * HEIGHT];
float global_time = 0;

#define SRC_SCALE 3

Olivec_Canvas render(float dt) {
  global_time += dt;

  float t = sinf(10 * global_time);

  Olivec_Canvas dst_canvas = olivec_canvas(dst, WIDTH, HEIGHT, WIDTH);

  olivec_fill(dst_canvas, 0xFF181818);

  int factor = 100;
  int w = 400 - t * factor;
  int h = 400 + t * factor;

  olivec_copy(olivec_canvas(png, png_width, png_height, png_width), dst_canvas,
              WIDTH / 2 - w / 2, HEIGHT - h, w, h);

  return dst_canvas;
}
