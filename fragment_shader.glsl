#version 330 core

in vec2 coordinates;
out vec4 color;

vec2 complex_add(vec2 a, vec2 b) {
    return a + b;
}

// (a.x + ia.y) * (b.x + ib.y) = (a.x * b.x - a.y * b.y, i(a.x * b.y + a.y * b.x))
vec2 complex_mult(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float complex_abs(vec2 a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

vec2 fractal_func(vec2 z, vec2 c) {
    return complex_add(complex_mult(z, z), c);
}

void main() {
    vec2 z = vec2(0, 0);
    for (int i = 0; i < 50; ++i) {
        z = fractal_func(z, coordinates);
    }
    if (complex_abs(z) < 2) {
        color = vec4(1, 1, 1, 1);
    } else {
        color = vec4(0, 0, 0, 1);
    }
    // color = vec4(coordinates, 0.0f, 1.0f);
}
