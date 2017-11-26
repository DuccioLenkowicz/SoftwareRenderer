#ifndef MESH_H
#define MESH_H

#define _CRT_SECURE_NO_WARNINGS 

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<cg_math.h>

struct mesh
{
    float *v;
    int *f;
    unsigned long v_count;
    unsigned long f_count;
    vec3_t pos;
    quat_t rot;
    vec3_t scale;
};

typedef struct mesh mesh_t;

mesh_t* mesh_load_obj(const char *);

mat4x4_t mesh_get_trasform(mesh_t *);

void mesh_destroy(mesh_t *);

const void *load_text(const char *);


#endif