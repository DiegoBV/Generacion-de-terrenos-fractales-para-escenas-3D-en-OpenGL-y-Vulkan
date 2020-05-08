const float LOW_STEPS = 2;
const float MID_STEPS = 5;
const float HIGH_STEPS = 12;

const int MAX_MARCHING_STEPS = 128;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.001;
const float MARCHING_STEP = 0.5;

float SDF(vec3 z);
float sceneDist = 0.0;

#include ..\\..\\Shaders\\terrainFunctions.c

float random(vec2 p)
{
    vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

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

float fbmH(vec2 p) {
  vec2 df = vec2(0.0);
  float f = 0.0;
  float w = 0.5;

  for (int i = 0; i < HIGH_STEPS; i++) {
    vec3 n = noise(p);
    df += n.yz;
    f += abs(w * n.x / (1.0 + dot(df, df)));
    w *= 0.5;
    p = 2. * terrainProps * p;
  }
  return f;
}

float fbmL(vec2 p) {
  vec2 df = vec2(0.0);
  float f = 0.0;
  float w = 0.5;

  for (int i = 0; i < LOW_STEPS; i++) {
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

vec3 normal(vec3 pos, float t) {
	vec2  eps = vec2( 0.002*t, 0.0 );
    return normalize( vec3( fbmH(pos.xz-eps.xy) - fbmH(pos.xz+eps.xy),
                            2.0*eps.x,
                            fbmH(pos.xz-eps.yx) - fbmH(pos.xz+eps.yx) ) );
}

struct light {
  vec3 lightPosition;
  vec3 amibnetColor;
  float ambientIntencity;
  vec3 directLightColor;
  vec3 directLightIntencity;
};

vec3 diffuseLight(vec3 k_d, vec3 p, vec3 eye, vec3 lightPos, vec3 lightIntensity) {
  vec3 N = normal(p, 0.01);
  vec3 L = normalize(lightPos - p);

  float dotLN = dot(L, N);

  if (dotLN < 0.0) {
    return vec3(0.0, 0.0, 0.0);
  }

  return lightIntensity * (k_d * dotLN);
}

vec3 calcLights(light data, vec3 p, vec3 eye) {
  vec3 ambientColor = data.ambientIntencity * data.amibnetColor;
  vec3 phongColor = diffuseLight(data.directLightColor, p, eye, data.lightPosition, data.directLightIntencity);

  return ambientColor + phongColor;
}

mat2 mm2(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
float tri(in float x){return clamp(abs(fract(x)-.5),0.01,0.49);}
vec2 tri2(in vec2 p){return vec2(tri(p.x)+tri(p.y),tri(p.y+tri(p.x)));}

float fbmAurora(vec2 p, float spd, float time) {
    float z = 1.8;
    float z2 = 2.5;
	float rz = 0.;
    p *= mm2(p.x * 0.06);
    vec2 bp = p;
	for (float i = 0.; i < 5.; i++ ) {
        vec2 dg = tri2(bp*1.85)*.75;
        dg *= mm2(time*spd);
        p -= dg/z2;

        bp *= 1.3;
        z2 *= .45;
        z *= .42;
		p *= 1.21 + (rz-1.0)*.02;
        
        rz += tri(p.x+tri(p.y))*z;
        p*= sin(time * 0.05) * cos(time * 0.01);
	}
    return clamp(1. / pow(rz * 20., 1.3), 0.,1.);
}


vec4 aurora( vec3 rd, vec2 fragCoord, float time) {
    vec4 col = vec4(0);
    vec4 avgCol = vec4(0);    

    for (float i=0.; i < 50.; i++) {
        float of = 0.006*random(fragCoord)*smoothstep(0.,15., i);
        float pt = ((.8+pow(i,1.4)*.002)) / (rd.y * 2. + 0.4);
        pt -= of;
    	vec3 bpos = 5.5 + pt * rd;
        vec2 p = bpos.zx;
        float rzt = fbmAurora(p, 0.06, time);
        vec4 col2 = vec4(0,0,0, rzt);
        col2.rgb = (sin(1.-vec3(2.15,-.5, 1.2) +i * 0.043) * 0.5 + 0.5)*rzt;
        avgCol = mix(avgCol, col2, .5);
        col += avgCol * exp2(-i*0.065 - 2.5) * smoothstep(0., 5., i);
    }
    col *= (clamp(rd.y*15.+.4,0.,1.));
 
    return smoothstep(0.,1.1,pow(col,vec4(1.))*1.5);
}

vec3 stars(vec2 p) {
    float r = fbmL(p * 20.  );
    float isStar = step(0.707, r);
    return vec3(r) * isStar;
}

void setSkyColor(vec2 uv, vec2 fragCoord, out vec3 color, vec3 dir, float time) {
   color = mix(vec3(0.006,0.026,0.095), vec3(0.007,0.011,0.035), uv.y);
   color += stars(dir.xz / dir.y);
   color += aurora(dir, fragCoord, time).rgb;
}

void setColor(vec3 p, vec3 n, out vec3 color) {
  float a = smoothstep(0.440 * n.y, 0.816 * n.y, fbmM(p.xz));
  vec3 ground = vec3(0.046,0.043,0.100);
  color = mix(vec3(1.), ground, a);  
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

vec3 getColor(vec3 p, vec3 cameraEye, vec3 rayDir, vec2 resolution, vec2 fragCoord, mat4 viewMat, float yDirection, float time)
{
    if (sceneDist < MAX_DIST)
    {
        light light1 = light(
            cameraEye + vec3(10., 150., 100.),                            // light position
            vec3(0.931, 0.975, 0.906), 0.412,                      // ambient color - ambient intencity
            vec3(0.254, 1.000, 0.777), vec3(0.162, 0.555, 0.560)); // direct light color - direct light intencity

        vec3 nor = normal(p, sceneDist);

        setColor(p, nor, color);

        color *= calcLights(light1, p, cameraEye);
    }
    else
    {
        vec2 uv = fragCoord / resolution;
        uv.y = yDirection * uv.y;
        setSkyColor(uv, fragCoord, color, rayDir, time);
    }

    color = pow(color, vec3(1. / 2.2)); // gamma correction
    color = smoothstep(0., 1., color);
    return color;
}