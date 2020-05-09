// "Mandelbox Tunnel" by dr2 - 2015
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

const float FRACTAL_DIST = 1.5;
const float FRACTAL_STEPS = 30;
const float FRACTAL_MIN_SCALE = 2;
const float FRACTAL_MAX_SCALE = 4.2;

const int MAX_MARCHING_STEPS = 50;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0003;
const float MARCHING_STEP = 0.5;

float qStep;
float scale = 2.8;

float PrCapsDf(vec3 p, float r, float h)
{
    return length(p - vec3(0., 0., h * clamp(p.z / h, -1., 1.))) - r;
}

vec3 HsvToRgb(vec3 c)
{
    vec3 p = abs(fract(c.xxx + vec3(1., 2. / 3., 1. / 3.)) * 6. - 3.);
    return c.z * mix(vec3(1.), clamp(p - 1., 0., 1.), c.y);
}

float MBoxDF(vec3 p)
{
    vec4 q, q0;
    q0 = vec4(p, 1.);
    q = q0;
    for (int n = 0; n < FRACTAL_STEPS; n++)
    {
        q.xyz = clamp(q.xyz, -1., 1.) * 2. - q.xyz;
        q *= scale / clamp(dot(q.xyz, q.xyz), 0.5, 1.);
        q += q0;
    }
    return length(q.xyz) / abs(q.w);
}

float SDF(vec3 p, vec3 origin)
{
    return max(MBoxDF(p), -PrCapsDf(p - origin, 0.2, 0.5));
}

float rayMarch(vec3 ro, vec3 rd, float start, float end)
{
    float d, h, s;
    d = 0.;
    s = 0.;
    for (int j = 0; j < MAX_MARCHING_STEPS; j++)
    {
        h = SDF(ro + d * rd, ro);
        d += h;
        ++s;
        if (h < EPSILON || d > end)
            break;
    }
    qStep = s / float(MAX_MARCHING_STEPS);
    return d;
}

vec3 ObjNf(vec3 p)
{
    const vec3 e = vec3(0.001, -0.001, 0.);
    vec4 v = vec4(SDF(p + e.xxx, p), SDF(p + e.xyy, p),
                  SDF(p + e.yxy, p), SDF(p + e.yyx, p));
    return normalize(vec3(v.x - v.y - v.z - v.w) + 2. * v.yzw);
}

vec3 getColor(float dist, vec3 ro, vec3 rd)
{
    vec3 ltDir, col, vn;
   
    ltDir = normalize(vec3(0.2, 1., -0.2));
    col = vec3(clamp(0.5 + 1.5 * rd.y, 0., 1.));
    if (dist < MAX_DIST)
    {
        ro += dist * rd;
        vn = ObjNf(ro);
        col = HsvToRgb(vec3(min(length(ro) / 5., 1.), 1.,
                            max(0., 1. - 0.5 * qStep * qStep)));
        col = col * (0.3 +
                     0.7 * max(dot(vn, ltDir), 0.)) +
              0.3 * pow(max(0., dot(ltDir, reflect(rd, vn))), 16.);
        col = clamp(col, 0., 1.);
    }
    return col;
}