#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    bool debug;
    float time;
    float yDirection;
    float playerRadius;
    vec2 resolution;
    vec3 fractalRotation;
    vec3 cameraEye;
    vec3 cameraFront;
    vec3 worldUp;
    vec3 playerPos;
    vec4 playerColor;
    mat4 viewMat;
    mat4 model;
    mat4 projection;
}
ubo;

layout(location = 0) out vec4 outColor;

float near = 0.1;
float far = 100.0;

// GEOMETRIES

mat3 rotateX(float angle, mat3 rotation)
{
    angle = radians(angle);
    rotation = mat3(
        1, 0, 0,
        0, cos(angle), -sin(angle),
        0, sin(angle), cos(angle));
    return rotation;
}
mat3 rotateY(float angle, mat3 rotation)
{
    angle = radians(angle);
    rotation = mat3(
        cos(angle), 0, sin(angle),
        0, 1, 0,
        -sin(angle), 0, cos(angle));
    return rotation;
}
mat3 rotateZ(float angle, mat3 rotation)
{
    angle = radians(angle);
    rotation = mat3(
        cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0,
        0, 0, 1);
    return rotation;
}


struct Sphere{
    float radius;
    vec3 center;
    mat3 rotation;
    vec4 color;
};

float sphereSDF(Sphere sphere, vec3 point) {
    return length(point - sphere.center) - sphere.radius;
}

struct Box{
    vec3 dimensions;
    vec3 center;
    mat3 rotation;
    vec4 color;
};

float boxSDF(Box box, vec3 point){
    point = point * box.rotation;
    vec3 q = (abs(point - box.center)) - box.dimensions;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

struct Cylinder{
    vec3 center;
    float radius;
    float height;
    mat3 rotation;
    vec4 color;
};

float cylinderSDF(Cylinder cylinder, vec3 point) {
    point = (point - cylinder.center) * cylinder.rotation;
    vec2 d = abs(vec2(length(point.xy),point.z)) - vec2(cylinder.radius, cylinder.height);
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

struct Plane{
    float height;
};

float planeSDF(Plane plane, vec3 point){
    return point.y + plane.height;
}

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
// "Mandelbox Tunnel" by dr2 - 2015
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

const float FRACTAL_DIST = 1.5;
const float FRACTAL_STEPS = 30;
const float FRACTAL_MIN_SCALE = 2;
const float FRACTAL_MAX_SCALE = 4.2;

float SDF(in vec3 p, out vec4 color, const in bool doColor, vec3 rotation);

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
    return mat3(c, 0, s, 0, 1, 0, -s, 0, c);
}

mat3 rotateZ(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(c, -s, 0, s, c, 0, 0, 0, 1);
}

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end, float time, vec3 rotation)
{
    //t is the point at which we are in the measuring of the distance
    float t = start;
    float c = 1.0;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        vec2 dist = SDF(eye + t * marchingDirection, time, rotation);
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

vec3 getColor(vec3 p, vec3 cameraEye, vec3 rayDir, vec2 resolution, vec2 fragCoord, mat4 viewMat, float yDirection, float time)
{
    return color;
}

mat3 makeBase(in vec3 w)
{
    float k = inversesqrt(1.0 - w.y * w.y);
    return mat3(vec3(-w.z, 0.0, w.x) * k,
                vec3(-w.x * w.y, 1.0 - w.y * w.y, -w.y * w.z) * k,
                w);
}

// http://iquilezles.org/www/articles/intersectors/intersectors.htm
vec2 sphIntersect(in vec3 ro, in vec3 rd, in float rad)
{
    float b = dot(ro, rd);
    float c = dot(ro, ro) - rad * rad;
    float h = b * b - c;
    if (h < 0.0)
        return vec2(-1.0);
    h = sqrt(h);
    return vec2(-b - h, -b + h);
}

// http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdCapsule(in vec3 p, in float b, in float r)
{
    float h = clamp(p.z / b, 0.0, 1.0);
    return length(p - vec3(0.0, 0.0, b) * h) - r; //*(0.2+1.6*h);
}

// modified Keinert et al's inverse Spherical Fibonacci Mapping
vec4 inverseSF(in vec3 p, const in float n)
{
    const float PI = 3.14159265359;
    const float PHI = 1.61803398875;

    float phi = min(atan(p.y, p.x), PI);
    float k = max(floor(log(n * PI * sqrt(5.0) * (1. - p.z * p.z)) / log(PHI + 1.)), 2.0);
    float Fk = pow(PHI, k) / sqrt(5.0);
    vec2 F = vec2(round(Fk), round(Fk * PHI));
    vec2 G = PI * (fract((F + 1.0) * PHI) - (PHI - 1.0));

    mat2 iB = mat2(F.y, -F.x, G.y, -G.x) / (F.y * G.x - F.x * G.y);
    vec2 c = floor(iB * 0.5 * vec2(phi, n * p.z - n + 1.0));

    float ma = 0.0;
    vec4 res = vec4(0);
    for (int s = 0; s < 4; s++)
    {
        vec2 uv = vec2(s & 1, s >> 1);
        float i = dot(F, uv + c);
        float phi = 2.0 * PI * fract(i * PHI);
        float cT = 1.0 - (2.0 * i + 1.0) / n;
        float sT = sqrt(1.0 - cT * cT);
        vec3 q = vec3(cos(phi) * sT, sin(phi) * sT, cT);
        float a = dot(p, q);
        if (a > ma)
        {
            ma = a;
            res.xyz = q;
            res.w = i;
        }
    }
    return res;
}

float SDF(in vec3 p, out vec4 color, const in bool doColor, vec3 rotation)
{
    p *= rotateX(rotation.z) * rotateZ(-rotation.x);
    float lp = length(p);
    float dmin = lp - 1.0;
    {
        vec3 w = p / lp;
        vec4 fibo = inverseSF(w, 700.0);
        float hh = 1.0 - smoothstep(0.05, 0.1, length(fibo.xyz - w));
        dmin -= 0.07 * hh;
        color = vec4(0.05, 0.1, 0.1, 1.0) * hh * (1.0 + 0.5 * sin(fibo.w * 111.1));
    }

    float s = 1.0;

    for (int i = 0; i < 3; i++)
    {
        float h = float(i) / float(3 - 1);

        vec4 f = inverseSF(normalize(p), 65.0 + h * 75.0);

        // snap
        p -= f.xyz;

        // orient to surface
        p = p * makeBase(f.xyz);

        // scale
        float scale = 6.6 + 2.0 * sin(111.0 * f.w);
        p *= scale;
        p.xy *= 1.2;

        //translate
        p.z -= 3.0 - length(p.xy) * 0.6 * sin(f.w * 212.1);

        // measure distance
        s *= scale;
        float d = sdCapsule(p, -6.0, 0.42);
        d /= s;

        if (d < dmin)
        {
            if (doColor)
            {
                color.w *= smoothstep(0.0, 5.0 / s, dmin - d);

                if (i == 0)
                {
                    color.xyz = vec3(0.425, 0.36, 0.1) * 1.1; // fall
                                                              //color.xyz = vec3(0.4,0.8,0.1);         // summer
                                                              //color.xyz = vec3(0.4,0.4,0.8);         // winter
                }

                color.zyx += 0.3 * (1.0 - sqrt(h)) * sin(f.w * 1111.0 + vec3(0.0, 1.0, 2.0));
                color.xyz = max(color.xyz, 0.0);
            }
            dmin = d;
        }
        else
        {
            color.w *= 0.4 * (0.1 + 0.9 * smoothstep(0.0, 1.0 / s, d - dmin));
        }
    }

    return dmin;
}

// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormal(in vec3 pos, in float ep, vec3 rotation)
{
    vec4 kk;
    vec3 n = vec3(0.0);
    for (int i = 0; i < 4; i++)
    {
        vec3 e = 0.5773 * (2.0 * vec3((((i + 3) >> 1) & 1), ((i >> 1) & 1), (i & 1)) - 1.0);
        n += e * SDF(pos + e * ep, kk, false, rotation);
    }
    return normalize(n);
}

// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float calcSoftshadow(in vec3 ro, in vec3 rd, float tmin, float tmax, const float k, vec3 rotation)
{
    vec2 bound = sphIntersect(ro, rd, 2.1);
    tmin = max(tmin, bound.x);
    tmax = min(tmax, bound.y);

    float res = 1.0;
    float t = tmin;
    for (int i = 0; i < 50; i++)
    {
        vec4 kk;
        float h = SDF(ro + rd * t, kk, false, rotation);
        res = min(res, k * h / t);
        t += clamp(h, 0.02, 0.20);
        if (res < 0.005 || t > tmax)
            break;
    }
    return clamp(res, 0.0, 1.0);
}

float rayMarch(in vec3 ro, in vec3 rd, in float tmin, in float tmax, vec3 rotation)
{
    vec4 kk;
    float t = tmin;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        vec3 p = ro + t * rd;
        float h = SDF(p, kk, false, rotation);
        if (abs(h) < EPSILON)
            break;
        t += h * MARCHING_STEP;
        if (t > tmax)
            break;
    }
    //if( t>tmax ) t=-1.0;

    return t;
}

vec3 getColor(float dist, vec3 ro, vec3 rd, vec2 resolution, vec2 fragCoord, float time, vec3 rotation)
{
    // render
    vec3 tot = vec3(0.0);
    vec3 col = vec3(0.0);

    if (dist > 0.0)
    {
        // local geometry
        vec3 pos = ro + dist * rd;
        vec3 nor = calcNormal(pos, 0.01, rotation);
        vec3 upp = normalize(pos);

        // color and occlusion
        vec4 mate;
        SDF(pos, mate, true, rotation);

        // lightning
        // // key ligh
        {
            // dif
            vec3 lig = normalize(vec3(1.0, 0.0, 0.7));
            float dif = clamp(0.5 + 0.5 * dot(nor, lig), 0.0, 1.0);
            float sha = calcSoftshadow(pos + 0.0001 * nor, lig, 0.0001, 2.0, 6.0, rotation);
            col += mate.xyz * dif * vec3(1.8, 0.6, 0.5) * 1.1 * vec3(sha, sha * 0.3 + 0.7 * sha * sha, sha * sha);
            // spec
            vec3 hal = normalize(lig - rd);
            float spe = clamp(dot(nor, hal), 0.0, 1.0);
            float fre = clamp(dot(-rd, lig), 0.0, 1.0);
            fre = 0.2 + 0.8 * pow(fre, 5.0);
            spe *= spe;
            spe *= spe;
            spe *= spe;
            col += 1.0 * (0.25 + 0.75 * mate.x) * spe * dif * sha * fre;
        }

        // back light
        {
            vec3 lig = normalize(vec3(-1.0, 0.0, 0.0));
            float dif = clamp(0.5 + 0.5 * dot(nor, lig), 0.0, 1.0);
            col += mate.rgb * dif * vec3(1.2, 0.9, 0.6) * 0.2 * mate.w;
        }

        // dome light
        {
            float dif = clamp(0.3 + 0.7 * dot(nor, upp), 0.0, 1.0);
            col += mate.xyz * dif * 3.0 * vec3(0.1, 0.1, 0.3) * mate.w * (0.2 + 0.8 * mate.w);
        }

        // fake sss
        {
            float fre = clamp(1.0 + dot(rd, nor), 0.0, 1.0);
            col += 0.3 * vec3(1.0, 0.3, 0.2) * mate.xyz * mate.xyz * fre * fre * mate.w;
        }

        // grade/sss
        {
            col = 2.0 * pow(col, vec3(0.7, 0.85, 1.0));
        }

        // exposure control
        col *= 0.7 + 0.3 * smoothstep(0.0, 25.0, abs(time - 31.0));
    }

    // gamma
    col = pow(col, vec3(0.4545));

    tot += col;

    return tot;
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord)
{
    vec2 uv = vec2(fragCoord.x - (size.x / 2.0), fragCoord.y - (size.y / 2.0));
    uv.y = ubo.yDirection * uv.y;
    float nearPlane = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -nearPlane));
}

float rayMarchDebug(vec3 eye, vec3 marchingDirection, float start, float end, Sphere sphere)
{
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        float dist = sphereSDF(sphere, eye + depth * marchingDirection);
        if (dist < EPSILON)
        {
            break;
        }
        depth += 0.5 * dist;
        if (depth >= end)
        {
            break;
        }
    }

    return depth;
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = ubo.cameraEye;
    vec3 rayDir = rayDirection(45.0, ubo.resolution, pixelCoord);
    rayDir = (ubo.viewMat * vec4(rayDir, 0.0)).xyz; //works

    mat3 identity = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    Sphere sphere; sphere.center = ubo.playerPos; sphere.radius = ubo.playerRadius; sphere.color = ubo.playerColor;
    sphere.rotation = identity;
    
    //tracing the ray (getting the distance of the closest object in the ray direction)
	
    float fractal = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST, ubo.fractalRotation);

    float pelota = rayMarchDebug(rayOrigin, rayDir, MIN_DIST, MAX_DIST, sphere);

    float distanceToSurface = 0.0f;
    if(ubo.debug){
        distanceToSurface = min(fractal, pelota);
    }
    else {
        distanceToSurface = fractal;
    }

    gl_FragDepth = distanceToSurface/(far + 1); // divide by far for demonstration
    
    //DEBUG (modelo)
    if(ubo.debug && pelota < fractal){
        outColor = sphere.color;
		return;
    }

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        outColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    outColor = vec4(getColor(distanceToSurface, rayOrigin, rayDir, ubo.resolution, pixelCoord, ubo.time, ubo.fractalRotation), 1);
}
