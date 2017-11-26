#ifndef DRAW_H
#define DRAW_H

#define _CRT_SECURE_NO_WARNINGS 
#define SDL_MAIN_HANDLED 1
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <mesh.h>
#include <cam.h>

struct draw_context
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    const uint8_t *keyboard_state;
    SDL_Texture *color_buffer;
    float *depth_buffer;
    unsigned int width;
    unsigned int height;
    float delta_time;
    float aspect_ratio;
    unsigned long long last;
};

typedef struct draw_context draw_context_t;

struct draw_color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

typedef struct draw_color draw_color_t;

draw_context_t *draw_context_new(const char *, unsigned int, unsigned int);
void draw_context_destroy(draw_context_t *);
void draw_context_blit(draw_context_t *);
void draw_context_clear(draw_context_t *, draw_color_t);
void draw_context_put_pixel(draw_context_t *, int, int, draw_color_t);
void draw_context_draw_line_bresenham(draw_context_t *, int, int, int, int, draw_color_t);
void draw_context_draw_mesh(draw_context_t *, mesh_t *, cam_t *, draw_color_t);
void draw_context_draw_triangle(draw_context_t *, float, float, float, float, float, float, draw_color_t);
uint8_t draw_context_get_key(draw_context_t *, SDL_Scancode);

#endif
