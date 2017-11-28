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
    memset(context->depth_buffer, 2.0f, sizeof(float) * context->width * context->height);
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

    // move left or right
    int sx = (x1 < x2) ? 1 : -1;

    // move down or up
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

        // backface culling
        vec4_t v14 = mat4x4_mul_vec4(model, vec4_create(x1, y1, z1, 1));
        vec4_t v24 = mat4x4_mul_vec4(model, vec4_create(x2, y2, z2, 1));
        vec4_t v34 = mat4x4_mul_vec4(model, vec4_create(x3, y3, z3, 1));

        vec3_t v1 = {v14.x, v14.y, v14.z};
        vec3_t v2 = {v24.x, v24.y, v24.z};
        vec3_t v3 = {v34.x, v34.y, v34.z};

        vec3_t t1 = (vec3_sub(v2, v1));
        vec3_t t2 = (vec3_sub(v3, v1));
        vec3_t p = vec3_add(vec3_scaled(vec3_normalized(t1), vec3_len(t1) * 0.5), vec3_add(vec3_scaled(vec3_normalized(t2), vec3_len(t2) * 0.5), v1));
        vec3_t n = vec3_cross(vec3_normalized(t1), vec3_normalized(t2));
        vec3_t cam_view = vec3_normalized(vec3_sub(p, cam->pos));
        if(vec3_dot(n, cam_view) > 0)
            continue;
        
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
        if(i%2 == 0)
        {
            draw_color_t color = {2 * i, 0, 255, 255};
            draw_context_draw_triangle(context, v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, color);
        }

        else if(i%3 == 0)
        {
            draw_color_t color = {40 * i, i, 255, 255};
            draw_context_draw_triangle(context, v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, color);
        }
    }
}

uint8_t draw_context_get_key(draw_context_t *context, SDL_Scancode code)
{
    return context->keyboard_state[code];  
}

void draw_context_draw_triangle(draw_context_t *context, float x0, float y0, float x1, float y1, float x2, float y2, draw_color_t color)
{
    // sort
    if(y1 > y0)
    {
        float old_x = x0;
        float old_y = y0;

        y0 = y1;
        x0 = x1;

        y1 = old_y;
        x1 = old_x;
    }
    if(y2 > y1)
    {
        float old_x = x1;
        float old_y = y1;

        y1 = y2;
        x1 = x2;

        y2 = old_y;
        x2 = old_x;
    }
    if(y1 > y0)
    {
        float old_x = x0;
        float old_y = y0;

        y0 = y1;
        x0 = x1;

        y1 = old_y;
        x1 = old_x;
    }


    // flat top
    if(y0 == y1)
    {
        // top leftmost first
        if(x1 < x0)
        {
            float old_x = x0;
            float old_y = y0;
            x0 = x1;
            y0 = y1;
            x1 = old_x;
            y1 = old_y;
        }

        draw_context_draw_triangle_flat_top(context, x0, y0, x1, y1, x2, y2, color);  
    }
    // flat bottom
    else if(y1 == y2)
    {
        // bottom leftmost second
        if(x2 < x1)
        {
            float old_x = x1;
            float old_y = y1;
            x1 = x2;
            y1 = y1;
            x2 = old_x;
            y2 = old_y;
        }

        draw_context_draw_triangle_flat_bottom(context, x0, y0, x1, y1, x2, y2, color);   
    }
    // generic
    else
    {
        float t = (y0 - y1) / (y0 - y2);
        float mid_x = t * x2 + (1 - t) * x0;
        float mid_y = t * y2 + (1 - t) * y0;

        // right major
        if(mid_x > x1)
        {
            draw_context_draw_triangle_flat_top(context, x1, y1, mid_x, mid_y, x2, y2, color);  
            draw_context_draw_triangle_flat_bottom(context, x0, y0, x1, y1, mid_x, mid_y, color);
        }
        // left major
        else
        {
            draw_context_draw_triangle_flat_top(context, mid_x, mid_y, x1, y1, x2, y2, color);  
            draw_context_draw_triangle_flat_bottom(context, x0, y0, mid_x, mid_y, x1, y1, color);
        }

    }
 
    // int p0x = (int)((x0 + 1) * context->width / 2.0f);
    // int p0y = (int)((-y0 + 1) * context->height / 2.0f);

    // int p1x = (int)((x1 + 1) * context->width / 2.0f);
    // int p1y = (int)((-y1 + 1) * context->height / 2.0f);

    // int p2x = (int)((x2 + 1) * context->width / 2.0f);
    // int p2y = (int)((-y2 + 1) * context->height / 2.0f);

    // draw_context_draw_line_bresenham(context, p0x, p0y, p1x, p1y, color);
    // draw_context_draw_line_bresenham(context, p0x, p0y, p2x, p2y, color);
    // draw_context_draw_line_bresenham(context, p1x, p1y, p2x, p2y, color);
}

void draw_context_draw_triangle_flat_top(draw_context_t *context, float x0, float y0, float x1, float y1, float x2, float y2, draw_color_t color)
{
    x0 = (x0 + 1.0f) * context->width / 2.0f;
    y0 = (1.0f - y0) * context->height / 2.0f;

    x1 = (x1 + 1.0f) * context->width / 2.0f;
    y1 = (1.0f - y1) * context->height / 2.0f;

    x2 = (x2 + 1.0f) * context->width / 2.0f;
    y2 = (1.0f - y2) * context->height / 2.0f;

    float m0 = (x2 - x0) / (y2 - y0);
    float m1 = (x2 - x1) / (y2 - y1);

    int scan_start_y  = (int)ceilf(y1 - 0.5f);
    int scan_end_y    = (int)ceilf(y2 - 0.5f);

    for(int j = scan_start_y; j < scan_end_y; j++)
    {
        // x = m * dy + x0
        float start_x_f = m0 * ((float)j + 0.5f - y0) + x0;
        float end_x_f   = m1 * ((float)j + 0.5f - y1) + x1;
        
        int scan_start_x  = (int)ceilf(start_x_f - 0.5f);
        int scan_end_x    = (int)ceilf(end_x_f - 0.5f);

        for(int i = scan_start_x; i < scan_end_x; i++)
        {
            draw_context_put_pixel(context, i, j, color);
        }
    }
}

void draw_context_draw_triangle_flat_bottom(draw_context_t *context, float x0, float y0, float x1, float y1, float x2, float y2, draw_color_t color)
{
    x0 = (x0 + 1.0f) * context->width / 2.0f;
    y0 = (1.0f - y0) * context->height / 2.0f;

    x1 = (x1 + 1.0f) * context->width / 2.0f;
    y1 = (1.0f - y1) * context->height / 2.0f;

    x2 = (x2 + 1.0f) * context->width / 2.0f;
    y2 = (1.0f - y2) * context->height / 2.0f;

    float m1 = (x1 - x0) / (y1 - y0);
    float m2 = (x2 - x0) / (y2 - y0);

    int scan_start_y  = (int)ceilf(y0 - 0.5f);
    int scan_end_y    = (int)ceilf(y2 - 0.5f);

    for(int j = scan_start_y; j < scan_end_y; j++)
    {
        // x = m * dy + x0
        float start_x_f = m1 * ((float)j + 0.5f - y0) + x0;
        float end_x_f   = m2 * ((float)j + 0.5f - y0) + x0;
        
        int scan_start_x  = (int)ceilf(start_x_f - 0.5f);
        int scan_end_x    = (int)ceilf(end_x_f - 0.5f);

        for(int i = scan_start_x; i < scan_end_x; i++)
        {
            draw_context_put_pixel(context, i, j, color);
        }
    }
}


