#ifndef CG_MATH_H
#define CG_MATH_H

typedef struct vec2
{
	float x;
	float y;
}vec2_t;

typedef struct vec3
{
	float x;
	float y;
	float z;
}vec3_t;

typedef struct vec4
{
    float x;
    float y;
    float z;
    float w;
}vec4_t;

typedef struct quat
{
	float w;
    vec3_t v;
}quat_t;

// column major
// right handed system
typedef struct mat4x4
{
    float m[16];
}mat4x4_t;

typedef struct ray
{
    vec3_t origin;
    vec3_t dir;
}ray_t;

vec2_t vec2_create(float, float);

vec2_t  vec2_add(vec2_t, vec2_t);

vec2_t  vec2_sub(vec2_t, vec2_t);

float  vec2_dot(vec2_t, vec2_t);

float vec2_cross(vec2_t, vec2_t);

vec2_t  vec2_mul(vec2_t, vec2_t);

vec2_t  vec2_scaled(vec2_t, float);

float  vec2_len(vec2_t);

float  vec2_len_sq(vec2_t);

void  vec2_normalize(vec2_t *);

vec2_t  vec2_normalized(vec2_t);

vec2_t  vec2_rotated(vec2_t, float);

vec2_t  vec2_complex_mul(vec2_t, vec2_t);


vec3_t vec3_create(float, float, float);

vec3_t vec3_add(vec3_t, vec3_t);

vec3_t vec3_sub(vec3_t, vec3_t);

float vec3_dot(vec3_t, vec3_t);

vec3_t vec3_cross(vec3_t, vec3_t);

vec3_t vec3_mul(vec3_t, vec3_t);

vec3_t vec3_scaled(vec3_t, float);

float vec3_len(vec3_t);

float vec3_len_sq(vec3_t);

void vec3_normalize(vec3_t *);

vec3_t vec3_normalized(vec3_t);

vec3_t vec3_reflected(vec3_t, vec3_t);

vec3_t vec3_refracted(vec3_t, vec3_t, float);


vec4_t vec4_create(float, float, float, float);

vec4_t vec4_add(vec4_t, vec4_t);

vec4_t vec4_sub(vec4_t, vec4_t);

float vec4_dot(vec4_t, vec4_t);

vec4_t vec4_mul(vec4_t, vec4_t);

vec4_t vec4_scaled(vec4_t, float);

float vec4_len(vec4_t);

float vec4_len_sq(vec4_t);

void vec4_normalize(vec4_t *);

vec4_t vec4_normalized(vec4_t);


quat_t quat_create(float, float, float, float);

//first around y, then around z, then around x
quat_t quat_from_euler(float, float, float);

//assumes normalized axis
quat_t quat_from_axis_angle(vec3_t, float);

quat_t quat_conjugated(quat_t);

void quat_conjugate(quat_t *);

// the second is applied first
quat_t quat_mul(quat_t, quat_t);

vec3_t quat_rotated_vec(vec3_t, quat_t);

quat_t quat_slerp(quat_t, quat_t, float);

quat_t quat_rot_from_to(vec3_t, vec3_t);


mat4x4_t mat4x4_create(float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float);

mat4x4_t mat4x4_mul(mat4x4_t, mat4x4_t);

vec4_t mat4x4_mul_vec4(mat4x4_t, vec4_t);

mat4x4_t mat4x4_rot(quat_t);

mat4x4_t mat4x4_scale(vec3_t);

mat4x4_t mat4x4_transl(vec3_t);

mat4x4_t mat4x4_model(vec3_t, vec3_t, quat_t);

mat4x4_t mat4x4_lookat(vec3_t, quat_t);

mat4x4_t mat4x4_ortho(float, float, float, float, float, float);

mat4x4_t mat4x4_persp(float, float, float ,float, float, float);

#endif




