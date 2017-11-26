#include<draw.h>
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
    mat4x4_t proj               = cam_get_proj(cam);
    mat4x4_t view               = cam_get_view(cam);
    mat4x4_t model              = mesh_get_trasform(mesh);
    mat4x4_t model_view         = mat4x4_mul(view, model);
    mat4x4_t model_view_proj    = mat4x4_mul(proj, model_view);
    

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
        vec4_t v[3];
        v[0] = vec4_create(x1, y1, z1, 1);
        v[1] = vec4_create(x2, y2, z2, 1);
        v[2] = vec4_create(x3, y3, z3, 1);

        for(int k = 0; k < 3; k++)
            v[k] = mat4x4_mul_vec4(model_view_proj, v[k]);

        // clipping
        
        //perspective division
        for(int k = 0; k < 3; k++)
            v[k] = vec4_scaled(v[k], 1 / v[k].w);


        // rasterize
        draw_context_draw_triangle(context, v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, color);
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

