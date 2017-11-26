#include <mesh.h>
#include <utils.h>

mat4x4_t mesh_get_trasform(mesh_t *mesh)
{
    return mat4x4_model(mesh->pos, mesh->scale, mesh->rot);
}

void mesh_destroy(mesh_t *mesh)
{
    free(mesh->v);
    free(mesh->f);
    free(mesh);
}

// improve this: use dynamc memory
mesh_t* mesh_load_obj(const char *file_name)
{
    mesh_t *mesh = malloc(sizeof(mesh_t));
    mesh->v = NULL;
    mesh->f = NULL;
    mesh->v_count = 0;
    mesh->f_count = 0;
    mesh->pos = vec3_create(0, 0, 0);
    mesh->rot = quat_create(0, 0, 0, 1);
    mesh->scale = vec3_create(1, 1, 1);

    const char *file = load_text(file_name);
    if(!file)
    {
        fprintf(stderr, "Couldn't open file: %s\n", file_name);
        mesh_destroy(mesh);
        return NULL;
    }

    unsigned long v_size = 500000;
    unsigned long f_size = 500000;

    mesh->v = malloc(sizeof(float) * v_size);
    mesh->f = malloc(sizeof(int) * f_size);
    
    const char *cur_line = file;

    while(cur_line)
    {
        char *next_line = strchr(cur_line, '\n');

        if(cur_line[0] == 'v' && cur_line[1] == ' ')
        {
            char *line_cpy = malloc((unsigned long)(next_line - cur_line) + 1);
            memcpy(line_cpy, cur_line, (unsigned long)(next_line - cur_line));
            line_cpy[next_line - cur_line + 1] = (char)0;
            
            int i = 0;
            for(char *token = strtok(line_cpy, " "); token; token = strtok(NULL, " "))
            {
                if(i++ > 0)
                {                
                    mesh->v[mesh->v_count++] = atof(token);
                }                
            }
            free(line_cpy);
        }
        else if(cur_line[0] == 'f')
        {
            char *line_cpy = malloc(next_line - cur_line + 1);
            memcpy(line_cpy, cur_line, next_line - cur_line);
            line_cpy[next_line - cur_line + 1] = (char)0;

            char tokens[6][1000];

            int i = 0;
            for(char *token = strtok(line_cpy, " "); token; token = strtok(NULL, " "))
            {
                memcpy(&(tokens[i][0]), token, strlen(token));
                tokens[i][strlen(token) + 1] = (char)0;
                i++;
            }

            for(i = 1; i < 5; i++)
            {
                int j = 0;
                for(char *token = strtok(&(tokens[i][0]), "/"); token; token = strtok(NULL, "/"))
                {
                    if(!j++)
                    {
                        mesh->f[mesh->f_count++] = atoi(token) - 1;
                    }
                }
            }
            
            free(line_cpy);
        }


        cur_line = next_line ? (next_line + 1) : NULL;
    }
    free((void *)file);
    return mesh;
}