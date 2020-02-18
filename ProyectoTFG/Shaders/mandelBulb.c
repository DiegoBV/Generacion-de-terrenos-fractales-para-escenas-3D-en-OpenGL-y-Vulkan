const int MAX_MARCHING_STEPS = 128;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;
const float  MARCHINGSTEP = 0.5;

const float  MAXMANDELBROTDIST = 1.5;
const float  MANDELBROTSTEPS = 5;

vec3 color = vec3(0.0,0.0,0.0);

// cosine based palette, 4 vec3 params
vec3 cosineColor( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 10.28318*(c*t+d) );
}
vec3 palette (float t) {
    return cosineColor( t, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(0.01,0.01,0.01),vec3(0.00, 0.15, 0.20) );
}

vec2 SDF(vec3 pos) {
    float Power = 3.0+4.0*(sin(time/15.0)+1.0);
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < MANDELBROTSTEPS ; i++) {
		r = length(z);
		if (r>MAXMANDELBROTDIST) break;
		
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

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end){
    //t is the point at which we are in the measuring of the distance
    float t = start;
    float c = 1.0;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        vec2 dist = SDF(eye + t * marchingDirection);
        //dist.x = sceneSDF(eye + t * marchingDirection);
        if (dist.y < EPSILON) {
            break;
        }
        t += MARCHINGSTEP * dist.x;
        c += dist.y;
        if (t >= end) {
            break;
        }
    }
    
    //rendering with a fog calculation (further is darker)
	//float fog = 1.0 / (1.0 + distanceToSurface.x * distanceToSurface.x * 0.1);

    color = palette(c);
    
    return t;
}

vec3 getColor(vec3 p){
    return color;
}