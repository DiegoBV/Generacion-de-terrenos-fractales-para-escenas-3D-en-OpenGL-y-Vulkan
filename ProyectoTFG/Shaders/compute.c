#version 430 core

layout  (local_size_x  =  2)  in;

layout(std430, binding=0) buffer Pos{
    float deltaTime;
    float velocity;
    vec3 direction;
    vec3 position;
    vec3 debug;
};

const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.001;
const float MARCHING_STEP = 0.5;
const int MAX_MARCHING_STEPS = 256;
const float MID_STEPS = 5;
const int COLLISION_SAMPLES = 8;
#define M_PI 3.1415926535897932384626433832795

float random(vec2 p)
{
    vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

float sceneDist = 0.0;

vec3 noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    vec2 df = 20.0*f*f*(f*(f-2.0)+1.0);
    f = f*f*f*(f*(f*6.-15.)+10.);

    float a = random(i + vec2(0.5));
    float b = random(i + vec2(1.5, 0.5));
    float c = random(i + vec2(.5, 1.5));
    float d = random(i + vec2(1.5, 1.5));

    float k = a - b - c + d;
    float n = mix(mix(a, b, f.x), mix(c, d, f.x), f.y);

    return vec3(n, vec2(b - a + k * f.y, c - a + k * f.x) * df);
}

int xorshift(in int value) {
    // Xorshift*32
    // Based on George Marsaglia's work: http://www.jstatsoft.org/v08/i14/paper
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    return value;
}

float nextFloat(inout int seed) {
    seed = xorshift(seed);
    // FIXME: This should have been a seed mapped from MIN..MAX to 0..1 instead
    return abs(fract(float(seed) / 3141.592653));
}

float nextFloatRange(inout int seed, in float max) {
    return nextFloat(seed) * max;
}

vec3[COLLISION_SAMPLES] fibonacci_sphere(vec3 position){ // codigo
    int seed = 8;
    float rnd = nextFloatRange(seed, 1) * COLLISION_SAMPLES;

    vec3 dirs[COLLISION_SAMPLES];
    float offset = 2.0/COLLISION_SAMPLES;

    float increment = M_PI * (3.0 - sqrt(5.0));

    for(int i = 0; i < COLLISION_SAMPLES; i++){
        float y = ((i * offset) - 1) + (offset / 2);
        float r = sqrt(1 - pow(y,2));

        float phi = (int(i + rnd) % COLLISION_SAMPLES) * increment;

        float x = cos(phi) * r;
        float z = sin(phi) * r;

        dirs[i] = normalize(vec3(x + position.x,y + position.y,z + position.z) - position);
    }

    return dirs; 
}

mat2 terrainProps = mat2(0.8,-0.6,0.6,0.8);

float fbmM(vec2 p) {
    vec2 df = vec2(0.0);
    float f = 0.0;
    float w = 0.5;

    for (int i = 0; i < MID_STEPS; i++) {
        vec3 n = noise(p);
        df += n.yz;
        f += abs(w * n.x / (1.0 + dot(df, df)));
        w *= 0.5;
        p = 2. * terrainProps * p;
    }
    return f;
}

float SDF(vec3 p) {
    float scene = p.y;
    
    float h = fbmM(p.xz);	
    scene -= h;

  	return scene;
}

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end)
{
    //t is the point at which we are in the measuring of the distance
    float t = start;
    float dist = 0.0;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        dist = SDF(eye + t * marchingDirection);

        if (dist < EPSILON * t || t > MAX_DIST) break;
        
        t += MARCHING_STEP * dist;
    }

    dist < EPSILON * t ? (sceneDist = t) : (sceneDist = MAX_DIST);

    return sceneDist;
}

void main(){
    position += velocity * deltaTime * direction;
    position += deltaTime * 0.8 * vec3(0.0, -1.0, 0.0); // gravedad

    vec3 directors[COLLISION_SAMPLES] = fibonacci_sphere(position);
    for(int i = 0; i < COLLISION_SAMPLES; i++){
        float dist = rayMarch(position, directors[i], MIN_DIST, MAX_DIST);

        if(dist < 0.1){
            position += abs(dist - 0.1) * -(directors[i]);
        }
    }
}