#version 330 core

#define MAX_ITER 1000

in vec2 coords;
out vec4 color;

uniform vec2 camera_corner;
uniform float camera_width;

vec2 complex_add(vec2 a, vec2 b) {
    return a + b;
}

// (a.x + ia.y) * (b.x + ib.y) = (a.x * b.x - a.y * b.y, i(a.x * b.y + a.y * b.x))
vec2 complex_mult(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float complex_squared_abs(vec2 a) {
    return a.x * a.x + a.y * a.y;
}

float complex_abs(vec2 a) {
    return sqrt(complex_squared_abs(a));
}

vec2 fractal_func(vec2 z, vec2 c) {
    return complex_add(complex_mult(z, z), c);
}

vec3 color_by_iter(int iter) {
    float proportion = (1.0 * iter) / MAX_ITER;
    vec3 base_color = vec3(1, 0.55, 0);
    return base_color * (1 - proportion);
}

void main() {
    vec2 z = vec2(0, 0);
    vec2 camera_coords = (coords * 0.5 + vec2(0.5, 0.5)) * camera_width + camera_corner;
    // vec2 camera_coords = vec2((coords.x * 0.5 + 0.5) * camera_width, coords.y * 0.5 + 0.5);
    // vec2 camera_coords = coords;
    
    for (int i = 0; i < MAX_ITER; ++i) {
        z = fractal_func(z, camera_coords);
        if (complex_squared_abs(z) >= 4) {
            color = vec4(color_by_iter(i + 1), 1);
            return;
        }
    }
    color = vec4(color_by_iter(MAX_ITER), 1);
    // color = vec4(coords, 0.0f, 1.0f);
}
