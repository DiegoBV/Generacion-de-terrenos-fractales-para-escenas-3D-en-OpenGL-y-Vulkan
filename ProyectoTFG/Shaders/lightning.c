// LIGHTNING METHODS

vec3 getNormal(vec3 p, float time){
    return normalize(vec3(
        SDF(vec3(p.x + EPSILON, p.y, p.z), time) - SDF(vec3(p.x - EPSILON, p.y, p.z), time),
        SDF(vec3(p.x, p.y + EPSILON, p.z), time) - SDF(vec3(p.x, p.y - EPSILON, p.z), time),
        SDF(vec3(p.x, p.y, p.z  + EPSILON), time) - SDF(vec3(p.x, p.y, p.z - EPSILON), time)
    ));
}

float getLight(vec3 p, float time){
    vec3 lightPos = vec3(0, 5, 0);
    lightPos.x += cos(time)*5.0;
    lightPos.z += sin(time)*5.0;
    vec3 light = normalize(lightPos - p);
    vec3 normal = getNormal(p, time);

    float dif = dot(normal, light);
    float distancia = rayMarch(p + (normal * EPSILON * 2.0), light, MIN_DIST, MAX_DIST, time);
    if(distancia < length(lightPos - p)) dif *= 0.1;
    return dif;
}

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity, float time) {
    vec3 N = getNormal(p, time);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye, float time) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(4.0 * sin(time),
                          2.0,
                          4.0 * cos(time));
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity, time);
    
    vec3 light2Pos = vec3(2.0 * sin(0.37 * time),
                          2.0 * cos(0.37 * time),
                          2.0);
    vec3 light2Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity, time);    
    return color;
}