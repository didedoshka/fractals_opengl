#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <malloc/_malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const unsigned int SHADER_MAX_SOURCE_SIZE = 1024 * 1024;
const unsigned int SHADER_MAX_LINE_SIZE = 180;
const GLuint WIDTH = 900, HEIGHT = 900;

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "out vec2 coords;\n"
    "void main()\n"
    "{\n"
    "   coords = position;\n"
    "   gl_Position = vec4(position, 0.0f, 1.0f);\n"
    "}\0";

char* load_shader_from_file(const char* path) {
    FILE* file;
    file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    char buf[SHADER_MAX_LINE_SIZE];
    char* res = malloc(SHADER_MAX_SOURCE_SIZE);

    while (fgets(buf, SHADER_MAX_LINE_SIZE, file)) {
        strcat(res, buf);
    }

    fclose(file);

    return res;
}

float cameraCorner[2] = {-2.2, -1.5};
float cameraWidth = 3;

GLint cameraCornerLocation;
GLint cameraWidthLocation;

float zoomRectangleFirstX;
float zoomRectangleFirstY;
float zoomRectangleSecondX;
float zoomRectangleSecondY;
float zoomRectangleLeft;
float zoomRectangleUp;
float zoomRectangleRight;
float zoomRectangleDown;

char drawZoomRectangle = 0;

GLint zoomRectangleLeftLocation;
GLint zoomRectangleUpLocation;
GLint zoomRectangleRightLocation;
GLint zoomRectangleDownLocation;
GLint drawZoomRectangleLocation;

double screen_to_device_x_coordinate(double x) {
    return (x / WIDTH) * 2 - 1;
}

double screen_to_device_y_coordinate(double y) {
    return -((y / HEIGHT) * 2 - 1);
}

void calculateZoomRectangleCoords() {
    float firstX = zoomRectangleFirstX;
    float firstY = zoomRectangleFirstY;
    float secondX = zoomRectangleSecondX;
    float secondY = zoomRectangleSecondY;

    float width = secondX - firstX;
    float height = secondY - firstY;

    float size = fminf(fabsf(width), fabsf(height));

    if (width < 0) {
        zoomRectangleLeft = firstX - size;
        zoomRectangleRight = firstX;
    } else {
        zoomRectangleLeft = firstX;
        zoomRectangleRight = firstX + size;
    }
    if (height < 0) {
        zoomRectangleDown = firstY - size;
        zoomRectangleUp = firstY;
    } else {
        zoomRectangleDown = firstY;
        zoomRectangleUp = firstY + size;
    }
}

void sendZoomRectangleCoords() {
    glUniform1f(zoomRectangleLeftLocation, zoomRectangleLeft);
    glUniform1f(zoomRectangleRightLocation, zoomRectangleRight);
    glUniform1f(zoomRectangleDownLocation, zoomRectangleDown);
    glUniform1f(zoomRectangleUpLocation, zoomRectangleUp);
    glUniform1i(drawZoomRectangleLocation, drawZoomRectangle);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    float currentXCursorPos = fmax(-1.0, fmin(1.0, screen_to_device_x_coordinate(xpos)));
    float currentYCursorPos = fmax(-1.0, fmin(1.0, screen_to_device_y_coordinate(ypos)));
    if (!drawZoomRectangle) {
        zoomRectangleFirstX = currentXCursorPos;
        zoomRectangleFirstY = currentYCursorPos;
    } else {
        zoomRectangleSecondX = currentXCursorPos;
        zoomRectangleSecondY = currentYCursorPos;
    }
    calculateZoomRectangleCoords();
    sendZoomRectangleCoords();

    printf("cursor position x: %f y: %f\n", currentXCursorPos, currentYCursorPos);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            drawZoomRectangle = 1;
            zoomRectangleSecondX = zoomRectangleFirstX;
            zoomRectangleSecondY = zoomRectangleFirstY;
        }
        if (action == GLFW_RELEASE) {
            // TODO: recalc camera corners and width
            drawZoomRectangle = 0;
            // if user wants to make another zoom from the spot he just finished
            zoomRectangleFirstX = zoomRectangleSecondX;
            zoomRectangleFirstY = zoomRectangleSecondY;
        }
        calculateZoomRectangleCoords();
        sendZoomRectangleCoords();
        printf("%d\n", drawZoomRectangle);
    }
}

int main(void) {
    if (glfwInit()) {
        printf("Started GLFW context, OpenGL 3.3\n");
    } else {
        printf("Failed to start GLFW context\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "didedoshka's fractal", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window) {
        printf("Created GLFW window\n");
    } else {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Initialized OpenGL context\n");
    } else {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    GLint width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* fragmentShaderPath = "fragment_shader.glsl";
    const char* fragmentShaderSource = load_shader_from_file(fragmentShaderPath);
    if (!fragmentShaderSource) {
        printf("Couln't load %s\n", fragmentShaderPath);
        return -1;
    }
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
        return -1;
    }
    // link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        return -1;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -1.0f, 1.0f,   // top left
        -1.0f, -1.0f,  // bottom left
        1.0f,  -1.0f,  // bottom right
        -1.0f, 1.0f,   // top left
        1.0f,  1.0f,   // top right
        1.0f,  -1.0f,  // bottom right
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    cameraCornerLocation = glGetUniformLocation(shaderProgram, "camera_corner");
    cameraWidthLocation = glGetUniformLocation(shaderProgram, "camera_width");
    zoomRectangleLeftLocation = glGetUniformLocation(shaderProgram, "zoom_rectangle_left_x");
    zoomRectangleUpLocation = glGetUniformLocation(shaderProgram, "zoom_rectangle_up_y");
    zoomRectangleRightLocation = glGetUniformLocation(shaderProgram, "zoom_rectangle_right_x");
    zoomRectangleDownLocation = glGetUniformLocation(shaderProgram, "zoom_rectangle_down_y");
    drawZoomRectangleLocation = glGetUniformLocation(shaderProgram, "draw_zoom_rectangle");

    glUniform2f(cameraCornerLocation, cameraCorner[0], cameraCorner[1]);
    glUniform1f(cameraWidthLocation, cameraWidth);

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
