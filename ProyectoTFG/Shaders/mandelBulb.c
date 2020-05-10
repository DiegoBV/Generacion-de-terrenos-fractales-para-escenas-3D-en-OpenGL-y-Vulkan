const float FRACTAL_DIST = 1.5;
const float FRACTAL_STEPS = 5;

vec2 SDF(vec3 z, float time, vec3 rotation);

#include ..\\..\\Shaders\\fractalFunctions.c

vec2 SDF(vec3 pos, float time, vec3 rotation) {
	pos *= rotateX(rotation.z) * rotateZ(-rotation.x);
    float Power = 3.0+4.0*(sin(15.0)+1.0);
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < FRACTAL_STEPS ; i++) {
		r = length(z);
		if (r>FRACTAL_DIST) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=pos;
	}

	return vec2(0.5*log(r)*r/dr, 50.0*pow(dr,0.128/float(MAX_MARCHING_STEPS)));
}