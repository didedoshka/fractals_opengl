#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <malloc/_malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

float cameraCorner[2] = {-2, -1.5};
float cameraWidth = 3;

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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
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

    GLint cameraCornerLocation = glGetUniformLocation(shaderProgram, "camera_corner");
    GLint cameraWidthLocation = glGetUniformLocation(shaderProgram, "camera_width");

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
