#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    vec2 resolution;
    float time;
    vec3 cameraEye;
    vec3 cameraFront;
    vec3 worldUp;
    mat4 viewMat;
} ubo;

layout(location = 0) out vec4 outColor;

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord);

const float FRACTAL_DIST = 1.5;
const float FRACTAL_STEPS = 7;
const float FRACTAL_MIN_SCALE = 2;
const float FRACTAL_MAX_SCALE = 4.2;

vec2 SDF(vec3 z);

const int MAX_MARCHING_STEPS = 128;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;
const float MARCHING_STEP = 0.5;

vec3 color = vec3(0.0, 0.0, 0.0);

// cosine based palette, 4 vec3 params
vec3 cosineColor( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 10.28318*(c*t+d) );
}

vec3 palette (float t) {
    return cosineColor( t, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(0.01,0.01,0.01),vec3(0.00, 0.15, 0.20) );
}

mat3 rotateX(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(1, 0, 0, 0, c, -s, 0, s, c);
}

mat3 rotateY(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(c, 0, -s, 0, 1, 0, s, 0, c);
}

mat3 rotateZ(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(c, -s, 0, s, c, 0, 0, 0, 1);
}

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end)
{
    //t is the point at which we are in the measuring of the distance
    float t = start;
    float c = 1.0;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        vec2 dist = SDF(eye + t * marchingDirection);
        //dist.x = sceneSDF(eye + t * marchingDirection);
        if (dist.y < EPSILON)
        {
            break;
        }
        t += MARCHING_STEP * dist.x;
        c += dist.y;
        if (t >= end)
        {
            break;
        }
    }

    color = palette(c);

    return t;
}

vec3 getColor(vec3 p)
{
    return color;
}

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

vec2 SDF(vec3 z)
{
    z *= rotateX(ubo.time / 8) * rotateZ(-ubo.time / 8);
    float scale = FRACTAL_MAX_SCALE * abs(sin(ubo.time/16)) + FRACTAL_MIN_SCALE;
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

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 uv = vec2(fragCoord.x - (size.x/2.0), fragCoord.y - (size.y/2.0));
    uv.y = -uv.y;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -z));
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = ubo.cameraEye;
    vec3 rayDir = rayDirection(45.0, ubo.resolution, pixelCoord);
    rayDir = (ubo.viewMat * vec4(rayDir, 0.0)).xyz; //works
    
    //tracing the ray (getting the distance of the closest object in the ray direction)
	
    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        outColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    outColor = vec4(getColor(p), 1.0);  
}