#define SCALE_DOWN_FACTOR 20
#include "vc.c"
#include "assets/Sadge.c"

#define WIDTH 960
#define HEIGHT 720
#define BACKGROUND_COLOR 0xFF181818

static uint32_t pixels[WIDTH * HEIGHT];
static float triangle_angle = 0;

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#define PI 3.14159265359

Olivec_Canvas render(float dt) {
  Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
  Olivec_Canvas Sadge =
      olivec_canvas(Sadge_pixels, Sadge_width, Sadge_height, Sadge_width);

  olivec_fill(oc, BACKGROUND_COLOR);

  // Triangle
  {
    triangle_angle += 0.5f * PI * dt;

    float ps[4][2];
    Uv uvs[4] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
    };
    float len = WIDTH / 4;
    for (size_t i = 0; i < 4; ++i) {
      ps[i][0] = WIDTH / 2 + cosf(PI / 2 * i + triangle_angle) * len;
      ps[i][1] = HEIGHT / 2 + sinf(PI / 2 * i + triangle_angle) * len;
    }
    for (size_t i = 0; i < 2; ++i) {
      int i1 = (i * 2 + 0) % 4;
      int i2 = (i * 2 + 1) % 4;
      int i3 = (i * 2 + 2) % 4;
      olivec_triangle3uv(oc, ps[i1][0], ps[i1][1], ps[i2][0], ps[i2][1],
                         ps[i3][0], ps[i3][1], uvs[i1], uvs[i2], uvs[i3], 1, 1,
                         1, Sadge);
    }
  }

  return oc;
}
