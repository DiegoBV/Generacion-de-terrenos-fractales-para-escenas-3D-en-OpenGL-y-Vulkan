const float FRACTAL_DIST = 1.5;
const float FRACTAL_STEPS = 7;
const float FRACTAL_MIN_SCALE = 2;
const float FRACTAL_MAX_SCALE = 4.2;

vec2 SDF(vec3 z, float time, vec3 rotation);

#include ..\\..\\Shaders\\fractalFunctions.c

vec3 boxFold(vec3 z, vec3 r)
{
    return clamp(z.xyz, -r, r) * 2.0 - z.xyz;
}

void sphereFold(inout vec3 z, inout float dz)
{
    float fixedRadius2 = 1.6;
    float minRadius2 = 0.4;
    float r2 = dot(z, z);
    if (r2 < minRadius2)
    {
        float temp = (fixedRadius2 / minRadius2);
        z *= temp;
        dz *= temp;
    }
    else if (r2 < fixedRadius2)
    {
        float temp = (fixedRadius2 / r2);
        z *= temp;
        dz *= temp;
    }
}

vec3 mengerFold(vec3 z)
{
    float a = min(z.x - z.y, 0.0);
    z.x -= a;
    z.y += a;
    a = min(z.x - z.z, 0.0);
    z.x -= a;
    z.z += a;
    a = min(z.y - z.z, 0.0);
    z.y -= a;
    z.z += a;
    return z;
}

vec2 SDF(vec3 z, float time, vec3 rotation)
{
    z *= rotateX(rotation.z) * rotateZ(-rotation.x);
    //z *= rotateX(time / 8) * rotateZ(-time / 8);
    float scale = FRACTAL_MAX_SCALE * abs(sin(time/16)) + FRACTAL_MIN_SCALE;
    vec3 offset = z;
    float dr = 1.0;
    float trap = 1e10;
    for (int i = 0; i < FRACTAL_STEPS; i++)
    {
        z = boxFold(z, vec3(0.8)); // Reflect
        z = boxFold(z, vec3(1.1)); // Reflect
        sphereFold(z, dr);         // Sphere Inversion
        z = scale * z + offset;    // Scale & Translate
        dr = dr * abs(scale) + 1.0;
        trap = min(trap, length(z));
    }

    float r = length(z);
    return vec2(r / abs(dr), 50.0*pow(dr,0.128/float(MAX_MARCHING_STEPS)));
}