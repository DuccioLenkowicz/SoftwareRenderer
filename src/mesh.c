#include <mesh.h>
#include <utils.h>

mesh_t *mesh_new()
{
    mesh_t *mesh = malloc(sizeof(mesh_t));
    mesh->v = NULL;
    mesh->f = NULL;
    mesh->pos   = vec3_create(0, 0, 0);
    mesh->rot   = quat_create(0, 0, 0, 1);
    mesh->scale = vec3_create(1, 1, 1);
    return mesh;
}

void mesh_destroy(mesh_t *mesh)
{
    float_buffer_destroy(mesh->v);
    int_buffer_destroy(mesh->f);
    free(mesh);
}

mat4x4_t mesh_get_trasform(mesh_t *mesh)
{
    return mat4x4_model(mesh->pos, mesh->scale, mesh->rot);
}

mesh_t* mesh_load_obj(const char *file_name)
{
    mesh_t *mesh = mesh_new();

    const char *file = load_text(file_name);
    if(!file)
    {
        fprintf(stderr, "Couldn't open file: %s\n", file_name);
        mesh_destroy(mesh);
        return NULL;
    }

    mesh->v = float_buffer_new(200, NULL);
    mesh->f = int_buffer_new(200, NULL);
    mesh->n = float_buffer_new(200, NULL);
    mesh->uv = float_buffer_new(200, NULL);
    
    const char *cur_line = file;

    while(cur_line)
    {
        char *next_line = strchr(cur_line, '\n');

        if(cur_line[0] == 'v')
        {
            char *line_cpy = malloc((unsigned long)(next_line - cur_line) + 1);
            memcpy(line_cpy, cur_line, (unsigned long)(next_line - cur_line));
            line_cpy[next_line - cur_line + 1] = (char)0;

            if(cur_line[1] == ' ')
            {
                int i = 0;
                for(char *token = strtok(line_cpy, " "); token; token = strtok(NULL, " "))
                {
                    if(i++ > 0)
                    {                
                        float value = atof(token);
                        float_buffer_add(mesh->v, value);
                    }                
                }
            }
            else if(cur_line[1] == 'n')
            {
                int i = 0;
                for(char *token = strtok(line_cpy, " "); token; token = strtok(NULL, " "))
                {
                    if(i++ > 0)
                    {                
                        float value = atof(token);
                        float_buffer_add(mesh->n, value);
                    }                
                }
            }
            else if(cur_line[1] == 't')
            {
                int i = 0;
                for(char *token = strtok(line_cpy, " "); token; token = strtok(NULL, " "))
                {
                    if(i++ > 0)
                    {                
                        float value = atof(token);
                        float_buffer_add(mesh->uv, value);
                    }                
                }
            }

            free(line_cpy);
        }
        else if(cur_line[0] == 'f')
        {
            char *line_cpy = malloc(next_line - cur_line + 1);
            memcpy(line_cpy, cur_line, next_line - cur_line);
            line_cpy[next_line - cur_line + 1] = (char)0;

            char tokens[4][100];

            int i = 0;
            for(char *token = strtok(line_cpy, " "); token; token = strtok(NULL, " "))
            {
                memcpy(&(tokens[i][0]), token, strlen(token));
                tokens[i][strlen(token) + 1] = (char)0;
                i++;
            }

            for(i = 1; i < 4; i++)
            {
                int j = 0;
                for(char *token = strtok(&(tokens[i][0]), "/"); token; token = strtok(NULL, "/"))
                {
                    if(j < 3)
                    {
                        int_buffer_add(mesh->f, atoi(token) - 1);
                    }
                    j++;
                }
            }
            
            free(line_cpy);
        }


        cur_line = next_line ? (next_line + 1) : NULL;
    }
    free((void *)file);
    return mesh;
}

void mesh_iter(mesh_t *mesh, triangle_t *triangle, int *face)
{
    int i = *face;
    int fv1 = mesh->f->data[i];
    int fn1 = mesh->f->data[i + 1];
    int fuv1 = mesh->f->data[i + 2];

    int fv2 = mesh->f->data[i + 3];
    int fn2 = mesh->f->data[i + 4];
    int fuv2 = mesh->f->data[i + 5];

    int fv3 = mesh->f->data[i + 6];
    int fn3 = mesh->f->data[i + 7];
    int fuv3 = mesh->f->data[i + 8];

    triangle->v[0].v = vec3_create(mesh->v->data[3 * fv1], mesh->v->data[3 * fv1 + 1], mesh->v->data[3 * fv1 + 2]);
    triangle->v[0].n = vec3_create(mesh->n->data[3 * fn1], mesh->n->data[3 * fn1 + 1], mesh->n->data[3 * fn1 + 2]);
    triangle->v[0].uv = vec2_create(mesh->uv->data[2 * fuv1], mesh->uv->data[2 * fuv1 + 1]);

    triangle->v[1].v = vec3_create(mesh->v->data[3 * fv2], mesh->v->data[3 * fv2 + 1], mesh->v->data[3 * fv2 + 2]);
    triangle->v[1].n = vec3_create(mesh->n->data[3 * fn2], mesh->n->data[3 * fn2 + 1], mesh->n->data[3 * fn2 + 2]);
    triangle->v[1].uv = vec2_create(mesh->uv->data[2 * fuv2], mesh->uv->data[fuv2 + 1]);

    triangle->v[2].v = vec3_create(mesh->v->data[3 * fv3], mesh->v->data[3 * fv3 + 1], mesh->v->data[3 * fv3 + 2]);
    triangle->v[2].n = vec3_create(mesh->n->data[3 * fn3], mesh->n->data[3 * fn3 + 1], mesh->n->data[3 * fn3 + 2]);
    triangle->v[2].uv = vec2_create(mesh->uv->data[2 * fuv1], mesh->uv->data[2 * fuv1 + 1]);

    *face += 9;
}
