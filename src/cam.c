#include<cam.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

cam_t *cam_new()
{
    // default persp 45 deg in y and x
    cam_t *cam = malloc(sizeof(cam_t));
    cam->pos = vec3_create(0, 0, 0);
    cam->rot = quat_create(0, 0, 0, 1);
    cam->top = 1;
    cam->bottom = -1;
    cam->right = 1;
    cam->left = -1;
    cam->near = 1;
    cam->far = 100;
    cam->is_ortho = 0;
    return cam;
}

void cam_destroy(cam_t *cam)
{
    free(cam);
}

void cam_set_persp(cam_t *cam, float fov, float aspect, float near, float far)
{
    cam->is_ortho = 0;
    float tan_fov = tan(fov / 2);
    cam->top = tan_fov;
    cam->bottom = -cam->top;
    cam->right = tan_fov * aspect;
    cam->left = -cam->right;
    cam->near = near;
    cam->far = far;
}

void cam_set_ortho(cam_t *cam, float left, float right, float top, float bottom, float near, float far)
{
    cam->is_ortho = 1;
    cam->top = top;
    cam->bottom = bottom;
    cam->right = right;
    cam->left = left;
    cam->near = near;
    cam->far = far;
}

mat4x4_t cam_get_view(cam_t *cam)
{
    return mat4x4_lookat(cam->pos, cam->rot);
}

mat4x4_t cam_get_proj(cam_t *cam)
{
    if(cam->is_ortho)
    {
        return mat4x4_ortho(cam->left, cam->right, cam->bottom, cam->top, cam->near, cam->far);
    }
    else
    {
        return mat4x4_persp(cam->left, cam->right, cam->bottom, cam->top, cam->near, cam->far);
    }
}

// world space ray from the camera to the center of a given pixel
ray_t cam_screen_point_to_ray(cam_t *cam, int x, int y, int width, int height)
{
    float sx = (2 * (x + 0.5f)/(float)width - 1) * cam->right;
    float sy = (-2 * (y + 0.5f)/(float)height + 1) * cam->top;
    vec3_t screen_p = vec3_create(sx, sy, cam->near);
    screen_p = quat_rotated_vec(screen_p, cam->rot);
    ray_t ray;
    ray.origin = cam->pos;
    ray.dir = screen_p;
    return ray;
}

