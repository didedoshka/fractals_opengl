#version 330 core

#define MAX_ITER 100
#define LIGHT_COEFF vec4(0.3, 0.3, 0.3, 0.0)

in vec2 coords;
out vec4 color;

uniform vec2 camera_corner;
uniform float camera_width;

uniform float zoom_rectangle_left_x;
uniform float zoom_rectangle_up_y;
uniform float zoom_rectangle_right_x;
uniform float zoom_rectangle_down_y;
uniform bool draw_zoom_rectangle;

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

vec3 color_by_iter_orange(int iter) {
    vec3 base_color = vec3(1, 0.55, 0);
    int colors_amount = 30;
    float proportion = (1.0 * iter) / colors_amount;
    return base_color * (1 - proportion);
}
vec3 color_by_iter_rgb(int iter) {
    if (iter % 3 == 0) {
        return vec3(1, 0, 0);
    } else if (iter % 3 == 1) {
        return vec3(0, 1, 0);
    } else {
        return vec3(0, 0, 1);
    }
}

vec3 color_by_iter_rainbow(int iter) {
    const int CYCLE_COLORS = 10;
    const int COLORS_AMOUNT = CYCLE_COLORS * 6 - 5;
    float step_diff = 1.0 / (CYCLE_COLORS - 1);
    vec3 colors[COLORS_AMOUNT];
    for (int i = 0; i < CYCLE_COLORS; ++i) {
        colors[0 * CYCLE_COLORS + i] = vec3(1.0, i * step_diff, 0.0);
    }
    for (int i = 1; i < CYCLE_COLORS; ++i) {
        colors[1 * CYCLE_COLORS + i - 1] = vec3(1.0 - i * step_diff, 1.0, 0.0);
    }
    for (int i = 1; i < CYCLE_COLORS; ++i) {
        colors[2 * CYCLE_COLORS - 1 + i - 1] = vec3(0.0, 1.0, i * step_diff);
    }
    for (int i = 1; i < CYCLE_COLORS; ++i) {
        colors[3 * CYCLE_COLORS - 2 + i - 1] = vec3(0.0, 1.0 - i * step_diff, 1.0);
    }
    for (int i = 1; i < CYCLE_COLORS; ++i) {
        colors[4 * CYCLE_COLORS - 3 + i - 1] = vec3(i * step_diff, 0.0, 1.0);
    }
    for (int i = 1; i < CYCLE_COLORS; ++i) {
        colors[5 * CYCLE_COLORS - 4 + i - 1] = vec3(1.0, 0.0, 1.0 - i * step_diff);
    }
    return colors[iter % COLORS_AMOUNT];
}

vec4 calculate_color_for_coordinates(vec2 camera_coords) {
    vec2 z = vec2(0, 0);
    for (int i = 0; i < MAX_ITER; ++i) {
        z = fractal_func(z, camera_coords);
        if (complex_squared_abs(z) >= 4) {
            return vec4(color_by_iter_rainbow(i), 1);
        }
    }
    return vec4(0, 0, 0, 1);
}

void main() {
    vec2 camera_coords = (coords * 0.5 + vec2(0.5, 0.5)) * camera_width + camera_corner;

    color = calculate_color_for_coordinates(camera_coords);

    if (draw_zoom_rectangle) {
        if (zoom_rectangle_left_x <= coords[0] && coords[0] <= zoom_rectangle_right_x && \
            zoom_rectangle_down_y <= coords[1] && coords[1] <= zoom_rectangle_up_y) {
            color += LIGHT_COEFF;
        }
    }
}
