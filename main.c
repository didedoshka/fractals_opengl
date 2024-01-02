#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned int SHADER_MAX_SOURCE_SIZE = 1024;
const unsigned int SHADER_MAX_LINE_SIZE = 180;
const GLuint WIDTH = 1200, HEIGHT = 800;

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "out vec2 coordinates;\n"
    "void main()\n"
    "{\n"
    "   coordinates = position;\n"
    "   gl_Position = vec4(position, 0.0f, 1.0f);\n"
    "}\0";

int load_shader_from_file(const char* path, char* res) {
    FILE* file;
    file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char buf[SHADER_MAX_LINE_SIZE];

    while (fgets(buf, SHADER_MAX_LINE_SIZE, file)) {
        strcat(res, buf);
    }

    fclose(file);

    return 1;
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
    char* fragmentShaderSource;
    fragmentShaderSource = malloc(SHADER_MAX_SOURCE_SIZE);
    if (!load_shader_from_file(fragmentShaderPath, fragmentShaderSource)) {
        printf("Couln't load %s\n", fragmentShaderPath);
    }
    printf("%s", fragmentShaderSource);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderSource, NULL);
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
    free(fragmentShaderSource);
    glDeleteShader(fragmentShader);

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

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
