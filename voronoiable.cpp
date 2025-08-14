#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct Color {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};


struct PointData {
    float x;
    float y;
};


struct Point {
    PointData pointData;
    Color color;
};

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


Color CreateRandomColor() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution dist(0.0f, 1.0f);

    return {
        dist(gen),
        dist(gen),
        dist(gen)
    };
}

GLuint CompileShader(const char * fileName, GLenum shaderType) {
    std::ifstream ifs;
    ifs.open(fileName, std::ifstream::in);
    std::string content = "";

    if(!ifs.is_open()) {
        printf("failed to open shader file :( path: %s\n", fileName);
        exit(1);
    }

    for(std::string line; std::getline(ifs, line);) {
        content += line;
        content.push_back('\n');
    }

    ifs.close();

    const char * contentRaw = content.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &contentRaw, NULL);
    glCompileShader(shader);

    GLint compiledSuccessfully;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiledSuccessfully);

    if (compiledSuccessfully != GL_TRUE) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader: %s compilation failed, details: %s\n", fileName, infoLog);
        exit(1);
    }

    return shader;
}

void DrawPoints(const std::vector<Point> & points) {

}


void AddPoint(std::vector<Point>& points, const float x, const float y) {
    points.push_back({x, y, CreateRandomColor()});
}


int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Voronoiable", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glDisable(GL_PROGRAM_POINT_SIZE);

    glPointSize(25);

    std::vector<Point> points = {};

    AddPoint(points, 0.1, 0.5);
    AddPoint(points, 0.4, 0.2);
    AddPoint(points, 0.3, 0);

    GLuint vertexShader = CompileShader("shaders/shader.vert", GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader("shaders/shader.frag", GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int compiledSuccessfully;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &compiledSuccessfully);
    if (!compiledSuccessfully) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Program linking failed, info: %s\n", infoLog);
        return 1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    GLuint vbo;

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLuint vao;

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)sizeof(PointData));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

	glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_DYNAMIC_DRAW);

        glDrawArrays(GL_POINTS, 0, points.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}