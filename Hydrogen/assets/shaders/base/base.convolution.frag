#version 330 core

in vec3 FragPosition;
uniform samplerCube Skybox;

out vec4 ResultColor;

const float PI = 3.14159265359;

void main() {
    // The world vector acts as the normal of a tangent surface
    // from the origin, aligned to FragPosition. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(FragPosition);

    vec3 irradiance = vec3(0.0);

    // tangent space calculation from origin point
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float delta = 0.025;
    float num_samples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += delta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += delta) {
            // spherical to cartesian (in tangent space)
            vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * N;

            irradiance += texture(Skybox, sample_vec).rgb * cos(theta) * sin(theta);
            num_samples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(num_samples));

    ResultColor = vec4(irradiance, 1.0);
}