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
    if(buffer->size == 0)
    {
        buffer->size = 4;
        buffer->data = malloc(sizeof(float) * buffer->size);
        if (!buffer->data)
        {
            return -1;
        }
        memset(buffer->data, 0, sizeof(float) * buffer->size);
    }
    else if(buffer->count == buffer->size)
    {
        uint64_t old_size = buffer->size;
        buffer->size *= 4;
        float *new_data = realloc(buffer->data, sizeof(float) * buffer->size);
        if (!new_data)
        {
            return -1;
        }
        buffer->data = new_data;
        uint64_t remains = (sizeof(float) * buffer->size) - (sizeof(float) * old_size);
        memset(buffer->data + old_size, 0, remains);
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
    if(buffer->size == 0)
    {
        buffer->size = 4;
        buffer->data = malloc(sizeof(int32_t) * buffer->size);
        if (!buffer->data)
        {
            return -1;
        }
        memset(buffer->data, 0, sizeof(int32_t) * buffer->size);
    }
    else if(buffer->count == buffer->size)
    {
        uint64_t old_size = buffer->size;
        buffer->size *= 4;
        int32_t *new_data = realloc(buffer->data, sizeof(int32_t) * buffer->size);
        if (!new_data)
        {
            return -1;
        }
        buffer->data = new_data;
        uint64_t remains = (sizeof(int32_t) * buffer->size) - (sizeof(int32_t) * old_size);
        memset(buffer->data + old_size, 0, remains);
    }

    buffer->data[buffer->count++] = value;
    return 0;
}
