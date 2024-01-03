#version 330 core

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

void main() {
    vec2 z = vec2(0, 0);
    vec2 camera_coords = (coords * 0.5 + vec2(0.5, 0.5)) * camera_width + camera_corner;
    // vec2 camera_coords = vec2((coords.x * 0.5 + 0.5) * camera_width, coords.y * 0.5 + 0.5);
    // vec2 camera_coords = coords;
    
    for (int i = 0; i < 1000; ++i) {
        z = fractal_func(z, camera_coords);
    }
    if (complex_squared_abs(z) < 4) {
        color = vec4(1, 1, 1, 1);
    } else {
        color = vec4(0, 0, 0, 1);
    }
    // color = vec4(coords, 0.0f, 1.0f);
}
