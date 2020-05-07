vec3 color = vec3(0.0, 0.0, 0.0);

mat3 rotateX(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(1, 0, 0, 0, c, -s, 0, s, c);
}

mat3 rotateY(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(c, 0, -s, 0, 1, 0, s, 0, c);
}

mat3 rotateZ(float angle)
{
    float c = cos(angle), s = sin(angle);
    return mat3(c, -s, 0, s, c, 0, 0, 0, 1);
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