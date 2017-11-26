#include <cg_math.h>
#include <math.h>

mat4x4_t mat4x4_identity = {1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f};

vec2_t vec2_create(float x, float y)
{
    vec2_t result;
    result.x = x;
    result.y = y;
    return result;
}

vec2_t vec2_add(vec2_t v1, vec2_t v2)
{
    return vec2_create(v1.x + v2.x, v1.y + v2.y);
}

vec2_t vec2_sub(vec2_t v1, vec2_t v2)
{
    return vec2_create(v1.x - v2.x, v1.y - v2.y);
}

float vec2_dot(vec2_t v1, vec2_t v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float vec2_cross(vec2_t v1, vec2_t v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

vec2_t vec2_mul(vec2_t v1, vec2_t v2)
{
    return vec2_create(v1.x * v2.x, v1.y * v2.y);
}

vec2_t vec2_scaled(vec2_t v, float s)
{
    return vec2_create(v.x * s, v.y * s);
}

float vec2_len(vec2_t v)
{
    return sqrt(vec2_len_sq(v));
}

float vec2_len_sq(vec2_t v)
{
    return vec2_dot(v, v);
}

void vec2_normalize(vec2_t *v)
{
    vec2_t temp = vec2_create(v->x, v->y);
    float len = vec2_len(temp);

    if(len == 0)
        return;

    v->x /= len;
    v->y /= len;
}

vec2_t vec2_normalized(vec2_t v)
{
    float len = vec2_len(v);

    if(len == 0)
        return v;
    
    return vec2_create(v.x / len, v.y / len);
}

vec2_t vec2_rotated(vec2_t v, float theta)
{
    float x = v.x * cos(theta) - v.y * sin(theta);
    float y = v.x * sin(theta) + v.y * cos(theta);

    return vec2_create(x, y);
}

vec2_t vec2_complex_mul(vec2_t v1, vec2_t v2)
{
    float r = v1.x * v2.x - v1.y * v2.y;
    float i = v1.x * v2.y + v1.y * v2.x;
    return vec2_create(r, i);
}

vec3_t vec3_create(float x, float y, float z)
{
    vec3_t result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

vec3_t vec3_add(vec3_t v1, vec3_t v2)
{
    return vec3_create(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec3_t vec3_sub(vec3_t v1, vec3_t v2)
{
    return vec3_create(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

float vec3_dot(vec3_t v1, vec3_t v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3_t vec3_cross(vec3_t v1, vec3_t v2)
{
    float x = v1.y * v2.z - v1.z * v2.y;
    float y = v1.z * v2.x - v1.x * v2.z;
    float z = v1.x * v2.y - v1.y * v2.x;

    return vec3_create(x, y, z);
}

vec3_t vec3_mul(vec3_t v1, vec3_t v2)
{
    return vec3_create(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

vec3_t vec3_scaled(vec3_t v, float s)
{
    return vec3_create(v.x * s, v.y * s, v.z * s);
}

float vec3_len(vec3_t v)
{
    return sqrt(vec3_len_sq(v));
}

float vec3_len_sq(vec3_t v)
{
    return vec3_dot(v, v);
}

void vec3_normalize(vec3_t *v)
{
    vec3_t temp = vec3_create(v->x, v->y, v->z);
    float len = vec3_len(temp);

    if(len == 0)
        return;

    v->x /= len;
    v->y /= len;
    v->z /= len;
}

vec3_t vec3_normalized(vec3_t v)
{
    float len = vec3_len(v);

    if(len == 0)
        return v;
    
    return vec3_create(v.x / len, v.y / len, v.z / len);
}

vec3_t vec3_reflected(vec3_t v, vec3_t n)
{
    return vec3_sub(v, vec3_scaled(n, 2 * vec3_dot(v, n)));    
}

vec3_t vec3_refracted(vec3_t i, vec3_t n, float ior)
{
    float cosi = vec3_dot(i, n);
    float etai = 1;
    float etat = ior; 
    if (cosi < 0) 
    { 
        cosi = -cosi; 
    } 
    else 
    {
        etai = ior;
        etat = 1;
        n = vec3_scaled(n, -1); 
    } 
    float eta = etai / etat; 
    float k = 1 - eta * eta * (1 - cosi * cosi); 

    if(k < 0)
        return vec3_create(0, 0, 0);

    return vec3_add(vec3_scaled(i, eta), vec3_scaled(n, eta * cosi - sqrtf(k)));
}

//TODO: implement from euler angle, slerp and rotation between vectors
// consider normalizing the result of mul to avoid floating point issues
quat_t quat_create(float x, float y, float z, float w)
{
    quat_t q;
    q.v = vec3_create(x, y, z);
    q.w = w;
    return q;
}

quat_t quat_from_axis_angle(vec3_t a, float angle)
{
    quat_t q;
    float s = sin(angle/2.0f);
    q.v = vec3_create(a.x * s, a.y * s, a.z * s);
    q.w = cos(angle/2.0f);

    float len = vec3_len_sq(q.v) + q.w * q.w;
    if(len > 1)
    {
        len = sqrt(len);
        q.v.x /= len;
        q.v.y /= len;
        q.v.z /= len;
        q.w /= len;
    }

    return q;
}

quat_t quat_conjugated(quat_t q)
{
    q.v = vec3_scaled(q.v, -1);
    return q;
}

void quat_conjugate(quat_t * q)
{
    q->v.x *= -1;
    q->v.x *= -1;
    q->v.z *= -1;
}

quat_t quat_mul(quat_t q1, quat_t q2)
{
    quat_t q;
    q.w = q1.w * q2.w - vec3_dot(q1.v, q2.v);
    q.v = vec3_add(vec3_add(vec3_scaled(q1.v, q2.w), vec3_scaled(q2.v, q1.w)), vec3_cross(q1.v, q2.v));
    float len = vec3_len_sq(q.v) + q.w * q.w;
    if(len > 1)
    {
        len = sqrt(len);
        q.v.x /= len;
        q.v.y /= len;
        q.v.z /= len;
        q.w /= len;
    }

    return q;
}

vec3_t quat_rotated_vec(vec3_t v, quat_t q)
{
    vec3_t cross = vec3_scaled(vec3_cross(q.v, v), 2.0f);
    float dot = vec3_dot(q.v, v);

    return vec3_add(v, vec3_add(vec3_scaled(cross, q.w), vec3_cross(q.v, cross)));
}

//TODO: consider optimizing a bit, lot of copy operation
mat4x4_t mat4x4_create(float m11, float m12, float m13, float m14, 
                        float m21, float m22, float m23, float m24,
                        float m31, float m32, float m33, float m34, 
                        float m41, float m42, float m43, float m44)
{
    mat4x4_t mat;
    //column 0
    mat.m[0] = m11;
    mat.m[1] = m21;
    mat.m[2] = m31;
    mat.m[3] = m41;

    //column 1
    mat.m[4] = m12;
    mat.m[5] = m22;
    mat.m[6] = m32;
    mat.m[7] = m42;

    //column 2
    mat.m[8]  = m13;
    mat.m[9]  = m23;
    mat.m[10] = m33;
    mat.m[11] = m43;

    //column 3
    mat.m[12] = m14;
    mat.m[13] = m24;
    mat.m[14] = m34;
    mat.m[15] = m44;

    return mat;
}

mat4x4_t mat4x4_mul(mat4x4_t left, mat4x4_t right)
{
    mat4x4_t mat;
    mat.m[0]  = left.m[0] * right.m[0] + left.m[4] * right.m[1] + left.m[8] * right.m[2] + left.m[12] * right.m[3]; 
    mat.m[1]  = left.m[1] * right.m[0] + left.m[5] * right.m[1] + left.m[9] * right.m[2] + left.m[13] * right.m[3]; 
    mat.m[2]  = left.m[2] * right.m[0] + left.m[6] * right.m[1] + left.m[10] * right.m[2] + left.m[14] * right.m[3]; 
    mat.m[3]  = left.m[3] * right.m[0] + left.m[7] * right.m[1] + left.m[11] * right.m[2] + left.m[15] * right.m[3]; 

    mat.m[4]  = left.m[0] * right.m[4] + left.m[4] * right.m[5] + left.m[8] * right.m[6] + left.m[12] * right.m[7]; 
    mat.m[5]  = left.m[1] * right.m[4] + left.m[5] * right.m[5] + left.m[9] * right.m[6] + left.m[13] * right.m[7]; 
    mat.m[6]  = left.m[2] * right.m[4] + left.m[6] * right.m[5] + left.m[10] * right.m[6] + left.m[14] * right.m[7]; 
    mat.m[7]  = left.m[3] * right.m[4] + left.m[7] * right.m[5] + left.m[11] * right.m[6] + left.m[15] * right.m[7]; 

    mat.m[8]  = left.m[0] * right.m[8] + left.m[4] * right.m[9] + left.m[8] * right.m[10] + left.m[12] * right.m[11]; 
    mat.m[9]  = left.m[1] * right.m[8] + left.m[5] * right.m[9] + left.m[9] * right.m[10] + left.m[13] * right.m[11]; 
    mat.m[10] = left.m[2] * right.m[8] + left.m[6] * right.m[9] + left.m[10] * right.m[10] + left.m[14] * right.m[11]; 
    mat.m[11] = left.m[3] * right.m[8] + left.m[7] * right.m[9] + left.m[11] * right.m[10] + left.m[15] * right.m[11]; 

    mat.m[12]  = left.m[0] * right.m[12] + left.m[4] * right.m[13] + left.m[8] * right.m[14] + left.m[12] * right.m[15];
    mat.m[13]  = left.m[1] * right.m[12] + left.m[5] * right.m[13] + left.m[9] * right.m[14] + left.m[13] * right.m[15];
    mat.m[14]  = left.m[2] * right.m[12] + left.m[6] * right.m[13] + left.m[10] * right.m[14] + left.m[14] * right.m[15]; 
    mat.m[15]  = left.m[3] * right.m[12] + left.m[7] * right.m[13] + left.m[11] * right.m[14] + left.m[15] * right.m[15]; 

    return mat;
}

vec4_t mat4x4_mul_vec4(mat4x4_t m, vec4_t v)
{
    vec4_t res;
    res.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w;
    res.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w;
    res.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
    res.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;
    return res;
}


mat4x4_t mat4x4_rot(quat_t q)
{
    return mat4x4_create(1.0f - 2.0f * (q.v.y * q.v.y + q.v.z * q.v.z),    2.0f * (q.v.x * q.v.y - q.v.z * q.w),            2.0f * (q.v.x * q.v.z + q.v.y * q.w),                0.0f,
                         2.0f * (q.v.z * q.w + q.v.x * q.v.y),             1.0f - 2.0f * (q.v.x * q.v.x + q.v.z * q.v.z),   2.0f * (q.v.y * q.v.z - q.v.x * q.w),                0.0f,
                         2.0f * (q.v.x * q.v.z - q.v.y * q.w),             2.0f * (q.v.y * q.v.z - q.v.x * q.w),            1.0f - 2.0f * (q.v.y * q.v.y + q.v.x * q.v.x),       0.0f,
                         0.0f,                                             0.0f,                                            0.0f,                                                1.0f);
}

mat4x4_t mat4x4_scale(vec3_t v)
{
    return mat4x4_create(v.x, 0.0f, 0.0f, 0.0f,
                         0.0f, v.y, 0.0f, 0.0f,
                         0.0f, 0.0f, v.z, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
}

mat4x4_t mat4x4_transl(vec3_t v)
{
    return mat4x4_create(1.0f, 0.0f, 0.0f, v.x,
                         0.0f, 1.0f, 0.0f, v.y,
                         0.0f, 0.0f, 1.0f, v.z,
                         0.0f, 0.0f, 0.0f, 1.0f);
}

mat4x4_t mat4x4_model(vec3_t p, vec3_t s, quat_t q)
{
    mat4x4_t rot    = mat4x4_rot(q);
    mat4x4_t scale  = mat4x4_scale(s);
    mat4x4_t transl = mat4x4_transl(p);

    // scale->rotate->translate
    return mat4x4_mul(transl, mat4x4_mul(rot, scale));
}

// world space forward : z+, cam space forward : z-
mat4x4_t mat4x4_lookat(vec3_t pos, quat_t rot)
{
    vec3_t xw = {1, 0, 0};
    vec3_t yw = {0, 1, 0};
    vec3_t zw = {0, 0, 1};
    
    // camera axis in world space
    vec3_t x = quat_rotated_vec(xw, rot);
    vec3_t y = quat_rotated_vec(yw, rot);
    vec3_t z = quat_rotated_vec(zw, rot);
    
    // reversed camera transform
    return mat4x4_create(x.x, y.x, -z.x, -vec3_dot(pos, x),
                         x.y, y.y, -z.y, -vec3_dot(pos, y),
                         x.z, y.z, -z.z,  vec3_dot(pos, z),
                         0.0f, 0.0f, 0.0f, 1.0f);
}

// double check this
mat4x4_t mat4x4_ortho(float left, float right, float bottom, float top, float z_near, float z_far)
{
    float w = right - left;
    if(w == 0)
    {
        return mat4x4_identity;
    }
    float h = top - bottom;
    if(h == 0)
    {
        return mat4x4_identity;
    }
    float d = z_far - z_near;
    if(d == 0)
    {
        return mat4x4_identity;
    }

    return mat4x4_create(2.0f/w,    0.0f,       0.0f,       -(right + left)/ w,
                         0.0f,      2.0f/h,     0.0f,       -(top + bottom)/ h,
                         0.0f,      0.0f,      -2.0f/d,     -(z_far + z_near)/d,
                         0.0f,      0.0f,       0.0f,       1.0f);
}

mat4x4_t mat4x4_persp(float left, float right, float bottom, float top, float z_near, float z_far)
{
    float w = right - left;
    if(w == 0)
    {
        return mat4x4_identity;
    }
    float h = top - bottom;
    if(h == 0)
    {
        return mat4x4_identity;
    }
    float d = z_far - z_near;
    if(d == 0)
    {
        return mat4x4_identity;
    }

    return mat4x4_create(2.0f * z_near /w,    0.0f,                           (right + left) / w,          0.0f,
                         0.0f,                2.0f * z_near / h,              (top + bottom) / h,          0.0f,
                         0.0f,                0.0f,                           -(z_far + z_near) / d,       -2.0f * z_far * z_near/ d, 
                         0.0f,                0.0f,                           -1.0f,                       0.0f);
}




