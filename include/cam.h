#include <cg_math.h>

struct cam
{
    vec3_t pos;
    quat_t rot;
    float top;
    float bottom;
    float left;
    float right;
    float near;
    float far;
    int is_ortho;
};

typedef struct cam cam_t;

cam_t *cam_new();
void *cam_destroy(cam_t *);
void cam_set_persp(cam_t *, float, float, float, float);
void cam_set_ortho(cam_t *, float, float, float, float, float, float);
mat4x4_t cam_get_view(cam_t *);
mat4x4_t cam_get_proj(cam_t *);
ray_t cam_screen_point_to_ray(cam_t *, int, int, int, int);



