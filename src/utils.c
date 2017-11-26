#include<utils.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const void *load_text(const char *file_name)
{
    FILE* stream = NULL;
    const void* buffer = NULL;
    long flen = 0;

    stream = fopen(file_name, "r");
    if(!stream)
    {
        fprintf(stderr, "Couldn't open file: %s\n", file_name);
        fclose(stream);
        return NULL;
    }

    fseek(stream, 0, SEEK_END);
    flen = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    buffer = malloc(flen);

    if(!buffer)
    {
        fprintf(stderr, "Couldn't allocate memory for the buffer");
        fclose(stream);
        return NULL;
    }
    else
    {
        fread((void*)buffer, 1, flen, stream);
    }

    fclose(stream);
    return buffer;
}

int max_int(int a, int b)
{
    if(a > b)
        return a;
    return b;
}

int min_int(int a, int b)
{
    if(a < b)
        return a;
    return b;
}