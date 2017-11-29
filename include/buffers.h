#ifndef BUFFERS_H
#define BUFFERS_H

#include<stdint.h>

struct float_buffer
{
    float *data;
    uint64_t size;
    uint64_t count;
};

struct int_buffer
{
    int32_t *data;
    uint64_t size;
    uint64_t count;
};

typedef struct int_buffer int_buffer_t;

typedef struct float_buffer float_buffer_t;

float_buffer_t *float_buffer_new(uint64_t, int *);
void float_buffer_destroy(float_buffer_t *);
int float_buffer_add(float_buffer_t *, float);

int_buffer_t *int_buffer_new(uint64_t, int *);
void int_buffer_destroy(int_buffer_t *);
int int_buffer_add(int_buffer_t *, int32_t);
#endif