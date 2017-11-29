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