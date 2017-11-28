#include<buffers.h>
#include<string.h>
#include<stdlib.h>

float_buffer_t *float_buffer_new(uint64_t size, int *err_code)
{
    if (err_code)
        *err_code = 0;
    float_buffer_t *buffer = malloc(sizeof(float_buffer_t));
    if (!buffer)
    {
        if (err_code)
            *err_code = -1;
        return NULL;
    }
    memset(buffer, 0, sizeof(float_buffer_t));

    if (size)
    {
        buffer->size = size;
        buffer->data = malloc(sizeof(float) * buffer->size);
        if (!buffer->data)
        {
            free(buffer);
            if (err_code)
                *err_code = -1;
            return NULL;
        }
        memset(buffer->data, 0, sizeof(float) * buffer->size);
    }

    return buffer;
}

void float_buffer_destroy(float_buffer_t *buffer)
{
    free(buffer->data);
    free(buffer);
}

int float_buffer_add(float_buffer_t *buffer, float value)
{
    if(!buffer->data)
    {
        buffer->size = 4;
        buffer->data = malloc(sizeof(float) * buffer->size);
        buffer->data[buffer->count++] = value;
        return 0;
    }
    if(buffer->count == buffer->size)
    {
        float *new_data = realloc(buffer->data, sizeof(float) * buffer->size * 4);
        if (!new_data)
        {
            return -1;
        }
        buffer->data = new_data;
        uint64_t remains = (sizeof(float) * buffer->size * 4) - (sizeof(float) * buffer->size);
        memset(buffer->data + sizeof(float) * buffer->size, 0, remains);
        buffer->size *= 4;
    }

    buffer->data[buffer->count++] = value;
    return 0;
}


int_buffer_t *int_buffer_new(uint64_t size, int *err_code)
{
    if (err_code)
        *err_code = 0;
    int_buffer_t *buffer = malloc(sizeof(int_buffer_t));
    if (!buffer)
    {
        if (err_code)
            *err_code = -1;
        return NULL;
    }
    memset(buffer, 0, sizeof(int_buffer_t));

    if (size)
    {
        buffer->size = size;
        buffer->data = malloc(sizeof(int32_t) * buffer->size);
        if (!buffer->data)
        {
            free(buffer);
            if (err_code)
                *err_code = -1;
            return NULL;
        }
        memset(buffer->data, 0, sizeof(int32_t) * buffer->size);
    }

    return buffer;

}
void int_buffer_destroy(int_buffer_t *buffer)
{
    free(buffer->data);
    free(buffer);
}
int int_buffer_add(int_buffer_t *buffer, int32_t value)
{
    if(!buffer->data)
    {
        buffer->size = 4;
        buffer->data = malloc(sizeof(int32_t) * buffer->size);
        buffer->data[buffer->count++] = value;
        return 0;
    }
    if(buffer->count == buffer->size)
    {
        int32_t *new_data = realloc(buffer->data, sizeof(int32_t) * buffer->size * 4);
        if (!new_data)
        {
            return -1;
        }
        buffer->data = new_data;
        uint64_t remains = (sizeof(int32_t) * buffer->size * 4) - (sizeof(int32_t) * buffer->size);
        memset(buffer->data + sizeof(int32_t) * buffer->size, 0, remains);
        buffer->size *= 4;
    }

    buffer->data[buffer->count++] = value;
    return 0;
}
