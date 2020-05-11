const int MAX_MARCHING_STEPS = 100;
const float MIN_DIST = 0.0;
const float MAX_DIST = 50.0;
const float EPSILON = 0.001;
const float MARCHING_STEP = 0.8;

const float FRACTAL_DIST = 1.5;
const float FRACTAL_STEPS = 30;
const float FRACTAL_MIN_SCALE = 2;
const float FRACTAL_MAX_SCALE = 4.2;

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
        vec4 fibo = inverseSF(w, 1500.0);
        float hh = 1.0 - smoothstep(0.05, 0.1, length(fibo.xyz - w));
        dmin -= 0.02 * hh;
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
        float scale = 8.0 + 2.0 * sin(111.0 * f.w);
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