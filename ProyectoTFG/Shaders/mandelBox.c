#version 330 core

// RAY MARCHING ALGORITHM
uniform vec2 resolution;
uniform float time;
uniform vec3 cameraEye;
uniform vec3 cameraFront;
uniform vec3 worldUp;
uniform mat4 viewMat;

const int MAX_MARCHING_STEPS = 128;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;

const int MAX_FRACTAL_STEPS = 20;

// predeclaramos las funciones por si las usan los archivos incluidos
float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end);
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord);

#include ..\\..\\Shaders\\scene0.c

void sphereFold(inout vec3 p, inout float dz) {
	float r2 = dot(p,p);
    float minRadius = 0.5;
    float fixedRadius = 1.0;

	if (r2<minRadius) { 
		// linear inner scaling
		float temp = (fixedRadius/minRadius);
		p *= temp;
		dz*= temp;
	} else if (r2<fixedRadius) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius/r2);
		p *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 p, inout float dz) {
	p = clamp(p *0.5+0.5, 0.0, 1.0) *4.0-2.0 - p;  
}

float DE(vec3 position) {
    float scale = 2.0;
    
    // vec3 offset = p;
	// float dr = 1.0;

	// for (int n = 0; n < MAX_FRACTAL_STEPS; n++) {
	// 	boxFold(p,dr);       // Reflect
	// 	sphereFold(p,dr);    // Sphere Inversion
 		
    //     p=scale*p + offset;  // Scale & Translate
    //     dr = dr*abs(scale)+1.0;
	// }
	// float r = length(p);
	// return r/abs(dr);

    // precomputed somewhere
    float minRadius = 0.5;
    float mR2 = minRadius * minRadius;
    vec4 scalevec = vec4(scale, scale, scale, abs(scale)) / mR2;
    float C1 = abs(scale-1.0), C2 = pow(abs(scale), float(1-MAX_FRACTAL_STEPS));

    // distance estimate
    vec4 p = vec4(position.xyz, 1.0), p0 = vec4(position.xyz, 1.0);  // p.w is knighty's DEfactor
    for (int i=0; i<MAX_FRACTAL_STEPS; i++) {
        p.xyz = clamp(p.xyz *0.5+0.5, 0.0, 1.0) *4.0-2.0 - p.xyz; // box fold: min3, max3, mad3
        float r2 = dot(p.xyz, p.xyz);  // dp3
        p.xyzw *= clamp(max(mR2/r2, mR2), 0.0, 1.0);  // sphere fold: div1, max1.sat, mul4
        p.xyzw = p*scalevec + p0;  // mad4
    }
    return (length(p.xyz) - C1) / p.w - C2;

//     float DEfactor = 0.0;
//     float x = p.x;
//     float y = p.y;
//     float z = p.z;

//     for (int n = 0; n < MAX_FRACTAL_STEPS; n++)
//     {
//         DEfactor = scale;

//         float fixedRadius = 1.0;
//         float fR2 = fixedRadius * fixedRadius;
//         float minRadius = 0.5;
//         float mR2 = minRadius * minRadius;

//         if (x > 1.0)
//             x = 2.0 - x;
//         else if (x < -1.0)
//             x = -2.0 - x;
//         if (y > 1.0)
//             y = 2.0 - y;
//         else if (y < -1.0)
//             y = -2.0 - y;
//         if (z > 1.0)
//             z = 2.0 - z;
//         else if (z < -1.0)
//             z = -2.0 - z;

//         float r2 = x * x + y * y + z * z;

//         if (r2 < mR2)
//         {
//             x = x * fR2 / mR2;
//             y = y * fR2 / mR2;
//             z = z * fR2 / mR2;
//             DEfactor = DEfactor * fR2 / mR2;
//         }
//         else if (r2 < fR2)
//         {
//             x = x * fR2 / r2;
//             y = y * fR2 / r2;
//             z = z * fR2 / r2;
//             DEfactor *= fR2 / r2;
//         }

//         x = x * scale + 0;
//         y = y * scale + 0;
//         z = z * scale + 0;
//         DEfactor *= scale;
//     }

//     return sqrt(x * x + y * y + z * z) / abs(DEfactor);
}

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end){
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = min(sceneSDF(eye + depth * marchingDirection), DE(eye + depth * marchingDirection));
        if (dist < EPSILON) {
			return depth;
        }
        depth += 0.5*dist;
        if (depth >= end) {
            return end;
        }
    }

    return end;
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 uv = vec2(fragCoord.x - (size.x/2.0), fragCoord.y - (size.y/2.0));
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -z));
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = cameraEye;
    vec3 rayDir = rayDirection(45.0, resolution, pixelCoord);
    rayDir = (viewMat * vec4(rayDir, 0.0)).xyz; //works

    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    float dif = getLight(p);

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    // Use the surface normal as the ambient color of the material
    vec3 K_a = (getNormal(p) + vec3(1.0)) / 2.0;
    vec3 K_d = K_a;
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 phongColor = phongIllumination(K_a, K_d, K_s, shininess, p, cameraEye);

    vec3 lightColor = vec3(dif);
    gl_FragColor = vec4(phongColor * lightColor, 1.0);
}
