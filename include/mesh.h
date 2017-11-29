#ifndef MESH_H
#define MESH_H

#define _CRT_SECURE_NO_WARNINGS 

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<cg_math.h>
#include<buffers.h>

struct mesh
{
    float_buffer_t *v;
    float_buffer_t *n;
    float_buffer_t *uv;

    int_buffer_t *f;
    
    vec3_t pos;
    quat_t rot;
    vec3_t scale;
};

struct vertex
{
    vec3_t v;
    vec3_t n;
    vec2_t uv;
};

struct triangle
{
    struct vertex v[3];
};

typedef struct triangle triangle_t;

typedef struct vertex vertex_t;

typedef struct mesh mesh_t;

mesh_t* mesh_load_obj(const char *);

mat4x4_t mesh_get_trasform(mesh_t *);

mesh_t* mesh_load_obj(const char *);

mesh_t * mesh_new();

void mesh_destroy(mesh_t *);

void mesh_iter(mesh_t *, triangle_t *, int *);

#endif