#ifndef OLIVE_C_
#define OLIVE_C_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef OLIVECDEF
#define OLIVECDEF static inline
#endif

#ifndef OLIVEC_AA_RES
#define OLIVEC_AA_RES 2
#endif

#define OLIVEC_SWAP(T, a, b)                                                   \
  do {                                                                         \
    T t = a;                                                                   \
    a = b;                                                                     \
    b = t;                                                                     \
  } while (0)
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x) * (x))

typedef struct {
  uint32_t *pixels;
  size_t height;
  size_t width;
  size_t stride;
} Olivec_Canvas;

#define OLIVEC_CANVAS_NULL ((Olivec_Canvas){0})
#define OLIVEC_PIXEL(oc, x, y) (oc).pixels[(y) * (oc).stride + (x)]

OLIVECDEF Olivec_Canvas olivec_canvas(uint32_t *pixels, size_t width,
                                      size_t height);
OLIVECDEF Olivec_Canvas olivec_subcanvas(Olivec_Canvas oc, int x, int y, int w,
                                         int h);
OLIVECDEF void olivec_blend_color(uint32_t *c1, uint32_t c2);
OLIVECDEF void olivec_fill(Olivec_Canvas oc, uint32_t color);
OLIVECDEF void olivec_rect(Olivec_Canvas oc, int x, int y, int w, int h,
                           uint32_t color);
OLIVECDEF void olivec_circle(Olivec_Canvas oc, int cx, int cy, int r,
                             uint32_t color);
// TODO: lines with different thicness
OLIVECDEF void olivec_line(Olivec_Canvas oc, int x1, int y1, int x2, int y2,
                           uint32_t color);
OLIVECDEF void olivec_triangle(Olivec_Canvas oc, int x1, int y1, int x2, int y2,
                               int x3, int y3, uint32_t color);
OLIVECDEF bool olivec_normalize_rect(int x, int y, int w, int h,
                                     size_t pixels_width, size_t pixels_height,
                                     int *x1, int *x2, int *y1, int *y2);

#endif // OLIVE_C_

#ifdef OLIVEC_IMPLEMENTATION
OLIVECDEF Olivec_Canvas olivec_canvas(uint32_t *pixels, size_t width,
                                      size_t height) {
  Olivec_Canvas oc = {
      .pixels = pixels,
      .width = width,
      .height = height,
      .stride = width,
  };
  return oc;
}

OLIVECDEF bool olivec_normalize_rect(int x, int y, int w, int h,
                                     size_t pixels_width, size_t pixels_height,
                                     int *x1, int *x2, int *y1, int *y2) {
  *x1 = x;
  *y1 = y;

  // Convert the rectangle to 2-points representation
  *x2 = *x1 + OLIVEC_SIGN(int, w) * (OLIVEC_ABS(int, w) - 1);
  if (*x1 > *x2)
    OLIVEC_SWAP(int, *x1, *x2);
  *y2 = *y1 + OLIVEC_SIGN(int, h) * (OLIVEC_ABS(int, h) - 1);
  if (*y1 > *y2)
    OLIVEC_SWAP(int, *y1, *y2);

  // Cull out invisible rectangle
  if (*x1 >= (int)pixels_width)
    return false;
  if (*x2 < 0)
    return false;
  if (*y1 >= (int)pixels_height)
    return false;
  if (*y2 < 0)
    return false;

  // Clamp the rectangle to the boundaries
  if (*x1 < 0)
    *x1 = 0;
  if (*x2 >= (int)pixels_width)
    *x2 = (int)pixels_width - 1;
  if (*y1 < 0)
    *y1 = 0;
  if (*y2 >= (int)pixels_height)
    *y2 = (int)pixels_height - 1;

  return true;
}

OLIVECDEF Olivec_Canvas olivec_subcanvas(Olivec_Canvas oc, int x, int y, int w,
                                         int h) {
  int x1, x2, y1, y2;
  if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &x1, &x2, &y1,
                             &y2))
    return OLIVEC_CANVAS_NULL;
  oc.pixels = &OLIVEC_PIXEL(oc, x1, y1);
  oc.width = x2 - x1 + 1;
  oc.height = y2 - y1 + 1;
  return oc;
}

OLIVECDEF void olivec_blend_color(uint32_t *c1, uint32_t c2) {
  uint32_t r1 = ((*c1) >> (0 * 8)) & 0xFF;
  uint32_t g1 = ((*c1) >> (1 * 8)) & 0xFF;
  uint32_t b1 = ((*c1) >> (2 * 8)) & 0xFF;
  uint32_t a1 = ((*c1) >> (3 * 8)) & 0xFF;

  uint32_t r2 = (c2 >> (0 * 8)) & 0xFF;
  uint32_t g2 = (c2 >> (1 * 8)) & 0xFF;
  uint32_t b2 = (c2 >> (2 * 8)) & 0xFF;
  uint32_t a2 = (c2 >> (3 * 8)) & 0xFF;

  r1 = (r1 * (255 - a2) + r2 * a2) / 255;
  if (r1 > 255)
    r1 = 255;
  g1 = (g1 * (255 - a2) + g2 * a2) / 255;
  if (g1 > 255)
    g1 = 255;
  b1 = (b1 * (255 - a2) + b2 * a2) / 255;
  if (b1 > 255)
    b1 = 255;

  *c1 = (r1 << (0 * 8)) | (g1 << (1 * 8)) | (b1 << (2 * 8)) | (a1 << (3 * 8));
}

OLIVECDEF void olivec_fill(Olivec_Canvas oc, uint32_t color) {
  for (size_t y = 0; y < oc.height; ++y) {
    for (size_t x = 0; x < oc.width; ++x) {
      OLIVEC_PIXEL(oc, x, y) = color;
    }
  }
}

OLIVECDEF void olivec_rect(Olivec_Canvas oc, int x, int y, int w, int h,
                           uint32_t color) {
  int x1, x2, y1, y2;
  if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &x1, &x2, &y1,
                             &y2))
    return;
  for (int x = x1; x <= x2; ++x) {
    for (int y = y1; y <= y2; ++y) {
      olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
    }
  }
}

OLIVECDEF void olivec_circle(Olivec_Canvas oc, int cx, int cy, int r,
                             uint32_t color) {
  int x1, y1, x2, y2;
  int r1 = r + OLIVEC_SIGN(int, r);
  if (!olivec_normalize_rect(cx - r1, cy - r1, 2 * r1, 2 * r1, oc.width,
                             oc.height, &x1, &x2, &y1, &y2))
    return;

  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      int count = 0;
      for (int sox = 0; sox < OLIVEC_AA_RES; ++sox) {
        for (int soy = 0; soy < OLIVEC_AA_RES; ++soy) {
          int res1 = (OLIVEC_AA_RES + 1);
          int dx = (x * res1 * 2 + 2 + sox * 2 - res1 * cx * 2 - res1);
          int dy = (y * res1 * 2 + 2 + soy * 2 - res1 * cy * 2 - res1);
          if (dx * dx + dy * dy <= res1 * res1 * r * r * 2 * 2)
            count += 1;
        }
      }
      uint32_t alpha = ((color & 0xFF000000) >> (3 * 8)) * count /
                       OLIVEC_AA_RES / OLIVEC_AA_RES;
      uint32_t updated_color = (color & 0x00FFFFFF) | (alpha << (3 * 8));
      olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), updated_color);
    }
  }
}

OLIVECDEF void olivec_line(Olivec_Canvas oc, int x1, int y1, int x2, int y2,
                           uint32_t color) {
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dx != 0) {
    int c = y1 - dy * x1 / dx;

    if (x1 > x2)
      OLIVEC_SWAP(int, x1, x2);
    for (int x = x1; x <= x2; ++x) {
      if (0 <= x && x < (int)oc.width) {
        int sy1 = dy * x / dx + c;
        int sy2 = dy * (x + 1) / dx + c;
        if (sy1 > sy2)
          OLIVEC_SWAP(int, sy1, sy2);
        for (int y = sy1; y <= sy2; ++y) {
          if (0 <= y && y < (int)oc.height) {
            olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
          }
        }
      }
    }
  } else {
    int x = x1;
    if (0 <= x && x < (int)oc.width) {
      if (y1 > y2)
        OLIVEC_SWAP(int, y1, y2);
      for (int y = y1; y <= y2; ++y) {
        if (0 <= y && y < (int)oc.height) {
          olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
        }
      }
    }
  }
}

OLIVECDEF void olivec_triangle(Olivec_Canvas oc, int x1, int y1, int x2, int y2,
                               int x3, int y3, uint32_t color) {
  if (y1 > y2) {
    OLIVEC_SWAP(int, x1, x2);
    OLIVEC_SWAP(int, y1, y2);
  }

  if (y2 > y3) {
    OLIVEC_SWAP(int, x2, x3);
    OLIVEC_SWAP(int, y2, y3);
  }

  if (y1 > y2) {
    OLIVEC_SWAP(int, x1, x2);
    OLIVEC_SWAP(int, y1, y2);
  }

  int dx12 = x2 - x1;
  int dy12 = y2 - y1;
  int dx13 = x3 - x1;
  int dy13 = y3 - y1;

  for (int y = y1; y <= y2; ++y) {
    if (0 <= y && (size_t)y < oc.height) {
      int s1 = dy12 != 0 ? (y - y1) * dx12 / dy12 + x1 : x1;
      int s2 = dy13 != 0 ? (y - y1) * dx13 / dy13 + x1 : x1;
      if (s1 > s2)
        OLIVEC_SWAP(int, s1, s2);
      for (int x = s1; x <= s2; ++x) {
        if (0 <= x && (size_t)x < oc.width) {
          olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
        }
      }
    }
  }

  int dx32 = x2 - x3;
  int dy32 = y2 - y3;
  int dx31 = x1 - x3;
  int dy31 = y1 - y3;

  for (int y = y2; y <= y3; ++y) {
    if (0 <= y && (size_t)y < oc.height) {
      int s1 = dy32 != 0 ? (y - y3) * dx32 / dy32 + x3 : x3;
      int s2 = dy31 != 0 ? (y - y3) * dx31 / dy31 + x3 : x3;
      if (s1 > s2)
        OLIVEC_SWAP(int, s1, s2);
      for (int x = s1; x <= s2; ++x) {
        if (0 <= x && (size_t)x < oc.width) {
          olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
        }
      }
    }
  }
}

#endif // OLIVEC_IMPLEMENTATION
