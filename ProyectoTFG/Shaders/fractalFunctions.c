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