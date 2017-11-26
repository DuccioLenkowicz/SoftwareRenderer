#include <draw.h>

draw_color_t black  = {0, 0, 0, 255};
draw_color_t red    = {255, 0, 0, 255};
draw_color_t yellow = {255, 255, 0, 255};
draw_color_t green  = {0, 255, 0, 255};

int main(int argc, char *argv[])
{
    draw_context_t *context = draw_context_new("Suzanne", 800, 600);

    vec3_t rot_axis = {0, 1, 0};
    vec3_normalize(&rot_axis);
    vec3_t pos = vec3_create(0, 0, 6);

    mesh_t *mesh = mesh_load_obj("suzanne.obj");
    cam_t *cam = cam_new();
    cam_set_persp(cam, 3.1415296f / 180 * 60, context->aspect_ratio, 1.0f, 100.0f);

    cam->pos  = vec3_create(0, 0, 0);
    mesh->pos = pos;
    mesh->rot = quat_from_axis_angle(rot_axis, 3.1415296f);
    
    float angle = 0.0f;

    for(;;)
    {
        draw_context_clear(context, black);
        SDL_PumpEvents();
        if(draw_context_get_key(context, SDL_SCANCODE_LEFT))
        {
            cam->pos = vec3_add(cam->pos, vec3_create(-context->delta_time * 2, 0, 0));
        }
        if(draw_context_get_key(context, SDL_SCANCODE_RIGHT))
        {
            cam->pos = vec3_add(cam->pos, vec3_create(context->delta_time * 2, 0, 0));
        }        
        if(draw_context_get_key(context, SDL_SCANCODE_UP))
        {
            cam->pos = vec3_add(cam->pos, vec3_create(0, context->delta_time * 2, 0));
        }
        if(draw_context_get_key(context, SDL_SCANCODE_DOWN))
        {
            cam->pos = vec3_add(cam->pos, vec3_create(0, -context->delta_time * 2, 0));
        }

        angle += context->delta_time;
        mesh->pos = vec3_add(quat_rotated_vec(vec3_create(0, 0, 2.0f), quat_from_axis_angle(rot_axis, 2 * angle)), pos);
        mesh->rot = quat_mul(quat_from_axis_angle(rot_axis, context->delta_time * 5), mesh->rot);
        cam->rot = quat_from_axis_angle(rot_axis, sin(angle) * 0.5);
        
        draw_context_draw_mesh(context, mesh, cam, yellow);
        draw_context_blit(context);
    }

    mesh_destroy(mesh);
    cam_destroy(cam);
    draw_context_destroy(context);
}