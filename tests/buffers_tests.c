#include<buffers.h>
#include<stdio.h>

int main(int argc, char **argv)
{
    float_buffer_t *buffer_f = float_buffer_new(70, NULL);

    if(!buffer_f)
        return -1;

    for(int i = 0; i < 10000; i++)
    {
        fprintf(stdout, "size: %llu, count: %llu \n", buffer_f->size, buffer_f->count);
        int ret = float_buffer_add(buffer_f, (float)i);
        
        if(ret < 0)
        {
            fprintf(stderr, "failed to reallocate buffer\n");
            return -1;
        }

        fprintf(stdout, "\n");
    }

    int_buffer_t *buffer_i = int_buffer_new(70, NULL);

    if(!buffer_i)
        return -1;

    for(int i = 0; i < 10000; i++)
    {
        fprintf(stdout, "size: %llu, count: %llu \n", buffer_i->size, buffer_i->count);
        int ret = int_buffer_add(buffer_i, i);
        
        if(ret < 0)
        {
            fprintf(stderr, "failed to reallocate buffer\n");
            return -1;
        }

        fprintf(stdout, "\n");
    }


    float_buffer_destroy(buffer_f);
    int_buffer_destroy(buffer_i);

    return 0;
}