#include <draw.h>
#include<cg_math.h>
#include<utils.h>

draw_context_t *draw_context_new(const char *title, unsigned int width, unsigned int height)
{
    draw_context_t *context = malloc(sizeof(draw_context_t));
    if (!context)
        return NULL;

    SDL_Init(SDL_INIT_VIDEO); 

    context->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    if(!context->window)
    {
        SDL_Quit();
        free(context);
        return NULL;
    }

    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!context->renderer)
    {
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        free(context);
        return NULL;
    }

    context->color_buffer = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if(!context->color_buffer)
    {
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        free(context);
        return NULL;
    }

    context->depth_buffer    = malloc(sizeof(float) * width * height);
    if(!context->depth_buffer)
    {
        SDL_DestroyTexture(context->color_buffer);
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        free(context);
        return NULL;
    }

    context->delta_time      = 0;
    context->width           = width;
    context->height          = height;
    context->aspect_ratio    = (float)width / (float)height;
    context->last            =  SDL_GetPerformanceCounter();
    context->keyboard_state  = SDL_GetKeyboardState(NULL);

    return context;
}

void draw_context_destroy(draw_context_t *context)
{
    SDL_DestroyTexture(context->color_buffer);

    SDL_DestroyRenderer(context->renderer);
   
    SDL_DestroyWindow(context->window);

    SDL_Quit();

    free(context);
}

void draw_context_blit(draw_context_t *context)
{
    SDL_SetRenderTarget(context->renderer, NULL);
    
    SDL_RenderCopy(context->renderer, context->color_buffer, NULL, NULL);

    SDL_RenderPresent(context->renderer);

    SDL_SetRenderTarget(context->renderer, context->color_buffer);
    
    unsigned long long now = SDL_GetPerformanceCounter();
    context->delta_time = (float)(now - context->last) / SDL_GetPerformanceFrequency();
    context->last = now;
}

void draw_context_clear(draw_context_t *context, draw_color_t color)
{
    SDL_SetRenderDrawColor(context->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(context->renderer);
}

void draw_context_put_pixel(draw_context_t *context, int x, int y, draw_color_t color)
{
    SDL_SetRenderDrawColor(context->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(context->renderer, x, y);
}

void draw_context_draw_line_bresenham(draw_context_t *context, int x1, int y1, int x2, int y2, draw_color_t color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    for(;;)
    {
        draw_context_put_pixel(context, x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;

        int err2 = err * 2;
        if (err2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (err2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_context_draw_mesh(draw_context_t *context, mesh_t *mesh, cam_t *cam, draw_color_t color)
{
    for(unsigned long i = 0; i < mesh->f_count; i += 3)
    {
        // find current face vertices
        unsigned long f1 = mesh->f[i];
        unsigned long f2 = mesh->f[i + 1];
        unsigned long f3 = mesh->f[i + 2];
        
        float x1 = mesh->v[3 * f1];
        float y1 = mesh->v[3 * f1 + 1];
        float z1 = mesh->v[3 * f1 + 2];

        float x2 = mesh->v[3 * f2];
        float y2 = mesh->v[3 * f2 + 1];
        float z2 = mesh->v[3 * f2 + 2];

        float x3 = mesh->v[3 * f3];
        float y3 = mesh->v[3 * f3 + 1];
        float z3 = mesh->v[3 * f3 + 2];
        
        // vertex shader
        mat4x4_t model      = mesh_get_trasform(mesh);
        mat4x4_t view       = cam_get_view(cam);
        mat4x4_t model_view = mat4x4_mul(view, model);

        vec4_t v14 = {x1, y1, z1, 1};
        vec4_t v24 = {x2, y2, z2, 1};
        vec4_t v34 = {x3, y3, z3, 1};

        v14 = mat4x4_mul_vec4(model_view, v14);
        v24 = mat4x4_mul_vec4(model_view, v24);
        v34 = mat4x4_mul_vec4(model_view, v34);

        mat4x4_t proj = cam_get_proj(cam);
        v14 = mat4x4_mul_vec4(proj, v14);
        v24 = mat4x4_mul_vec4(proj, v24);
        v34 = mat4x4_mul_vec4(proj, v34);

        
        // perspective division
        vec3_t v1 = {v14.x, v14.y, v14.z};
        vec3_t v2 = {v24.x, v24.y, v24.z};
        vec3_t v3 = {v34.x, v34.y, v34.z};

        v1.x /= v14.w;
        v1.y /= v14.w;

        v2.x /= v24.w;
        v2.y /= v24.w;

        v3.x /= v34.w;
        v3.y /= v34.w;

        // clipping

        // rasterize
        draw_context_draw_triangle(context, v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, color);
    }
}

uint8_t draw_context_get_key(draw_context_t *context, SDL_Scancode code)
{
    return context->keyboard_state[code];  
}

void draw_context_draw_triangle(draw_context_t *context, float x1, float y1, float x2, float y2, float x3, float y3, draw_color_t color)
{
    // viewport transform
    int p1x = (int)((x1 + 1) * context->width / 2.0f);
    int p1y = (int)((-y1 + 1) * context->height / 2.0f);

    int p2x = (int)((x2 + 1) * context->width / 2.0f);
    int p2y = (int)((-y2 + 1) * context->height / 2.0f);

    int p3x = (int)((x3 + 1) * context->width / 2.0f);
    int p3y = (int)((-y3 + 1) * context->height / 2.0f);

    int topx, topy, midx, midy, bottomx, bottomy;

    int max_x = max_int(p1x, max_int(p2x, p3x));
    int min_x = min_int(p1x, min_int(p2x, p3x));
    int max_y = max_int(p1y, max_int(p2y, p3y));
    int min_y = min_int(p1y, min_int(p2y, p3y));
    for(int j = min_y; j <= max_y; j++)
    {
        for(int i = min_x; i <= max_x; i++)
        {
            
        }
    }
    
    draw_context_draw_line_bresenham(context, p1x, p1y, p2x, p2y, color);
    draw_context_draw_line_bresenham(context, p1x, p1y, p3x, p3y, color);
    draw_context_draw_line_bresenham(context, p2x, p2y, p3x, p3y, color);
}

