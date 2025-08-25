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
    GLfloat x;
    GLfloat y;
};


struct Point {
    PointData pointData;
    Color color;
};


struct Triangle {
    PointData pd1;
    PointData pd2;
    PointData pd3;
    Color color;
};


struct LineEq {
    GLfloat a;
    GLfloat b;
};


LineEq GetLineEquation(const PointData & p1, const PointData & p2){
    // TODO: handle situation if x1 == x2

    LineEq output = {};

    output.a = (p1.y - p2.y) / (p1.x - p2.x);
    output.b = p1.y - output.a * p1.x;

    return output;
}


PointData GetIntersectionPoint(const LineEq& le1, const LineEq& le2) {
    PointData output = {};

    output.x = (le2.b - le1.b) / (le1.a - le2.a);
    output.y = output.x * le1.a + le1.b;

    return output;
}


bool DoLinesIntersect(
    const PointData& l1p1,
    const PointData& l1p2,
    const PointData& l2p1,
    const PointData& l2p2
) {
    LineEq lineEq1 = GetLineEquation(l1p1, l1p2);
    LineEq lineEq2 = GetLineEquation(l2p1, l2p2);

    // TODO:: use std::pair to handle error if lines do not intersect
    PointData intersectionPoint = GetIntersectionPoint(lineEq1, lineEq2);

    // If if exists on one section it definitely does on the other (and lines do intersect)
    return intersectionPoint.x > std::fmin(l1p1.x, l1p2.x) &&
        intersectionPoint.x < std::fmax(l1p1.x, l1p2.x);
}


bool DoPolygonsIntersect(const std::vector<PointData> & first, const std::vector<PointData> & second) {
    for (size_t i = 0; i < first.size(); i++) {
        size_t first_p1_index = i;
        size_t first_p2_index = i == first.size() - 1 ? 0 : i + 1;

        PointData first_p1 = first[first_p1_index];
        PointData first_p2 = first[first_p2_index];

		for (size_t j = 0; j < second.size(); j++) {
			size_t second_p1_index = i;
			size_t second_p2_index = i == second.size() - 1 ? 0 : i + 1;

			PointData second_p1 = second[second_p1_index];
			PointData second_p2 = second[second_p2_index];

            if (DoLinesIntersect(first_p1, first_p2, second_p1, second_p2)) return true;
		}
    }

    return false;
}


GLfloat CalculatePointToLineDistance(const PointData & pointData, const LineEq & lineEquation) {
    LineEq perpendicularLine = {};

    perpendicularLine.b = pointData.y - (pointData.x / lineEquation.a);
    perpendicularLine.a = (pointData.y - perpendicularLine.b) / pointData.x;

    const PointData intersectionPoint = GetIntersectionPoint(lineEquation, perpendicularLine);

    return CalculateDistance(pointData, intersectionPoint);
}


GLfloat GetTriangleArea(const PointData & p1, const PointData & p2, const PointData & p3) {
    const LineEq line = GetLineEquation(p1, p2);

    const GLfloat a = CalculateDistance(p1, p2);

    const GLfloat h = CalculatePointToLineDistance(p3, line);

    return (a * h) / 2;
}


PointData CalculateCenterOfGravity(const std::vector<Point> & points) {
    PointData sum = {0, 0};

    for (const auto& point : points) {
		sum.x += point.pointData.x;
		sum.y += point.pointData.y;
    }

    return {
        sum.x / points.size(),
        sum.y / points.size(),
    };
}


GLfloat CalculateDistance(const PointData & pd1, const PointData & pd2) {
    return std::sqrt(std::pow(pd1.x - pd2.x, 2) + std::pow(pd1.y - pd2.y, 2));
}


std::vector<std::vector<PointData>> ExtractPolygons(const std::vector<Point> & points) {
    std::vector<Point> pointsCopy(points);

    std::vector<std::vector<PointData>> output = {};

    // easier version - triangles

    // sort points from bottom-left to top-right
    PointData bottomLeft = { -1, -1 };

    std::sort(pointsCopy.begin(), pointsCopy.end(), [bottomLeft](const Point & p1, const Point & p2) {
        GLfloat firstDistance = CalculateDistance(bottomLeft, p1.pointData);
        GLfloat secondDistance = CalculateDistance(bottomLeft, p2.pointData);

        return firstDistance < secondDistance;
	});

    // for every point find best two points to create triangle

    std::vector<std::vector<PointData>> triangles = {};

    bool isCurrentBest = false;
    PointData currentBestP1 = {};

    return output;
}


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


GLuint CreateShaderProgram(const std::vector<GLuint>& shaders) {
    GLuint shaderProgram = glCreateProgram();

    for (const auto shader : shaders) {
		glAttachShader(shaderProgram, shader);
    }

	glLinkProgram(shaderProgram);

	int compiledSuccessfully;
	char infoLog[512];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &compiledSuccessfully);
	if (!compiledSuccessfully) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "Program linking failed, info: %s\n", infoLog);
		exit(1);
	}

    for (const auto shader : shaders) {
		glDeleteShader(shader);
    }

    return shaderProgram;
}


void AddPoint(std::vector<Point>& points, const GLfloat x, const GLfloat y) {
    points.push_back({x, y, CreateRandomColor()});
}


std::vector<Triangle> CreateTrianglesFromPoints(const std::vector<Point> & points) {
    std::vector<Triangle> triangles = {};

    for (const Point & point : points) {
        Triangle triangle1 = {};

        triangle1.pd1.x = point.pointData.x - 0.05;
        triangle1.pd1.y = point.pointData.y;
        triangle1.pd2.x = point.pointData.x - 0.1;
        triangle1.pd2.y = point.pointData.y;
        triangle1.pd3.x = point.pointData.x - 0.075;
        triangle1.pd3.y = point.pointData.y - 0.05;

        triangle1.color = CreateRandomColor();
        triangles.push_back(triangle1);
    }

    return triangles;
}


void InitializePointsAttribPointers() {
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)sizeof(PointData));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}


std::vector<Point> TransformTrianglesIntoPoints(const std::vector<Triangle> & triangles) {
    std::vector<Point> output;

    for (const auto & triangle : triangles) {
        Point point1 = {};
        Point point2 = {};
        Point point3 = {};

        point1.pointData = triangle.pd1;
        point1.color = triangle.color;

        point2.pointData = triangle.pd2;
        point2.color = triangle.color;

        point3.pointData = triangle.pd3;
        point3.color = triangle.color;

        output.push_back(point1);
        output.push_back(point2);
        output.push_back(point3);
    }

    return output;
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

    glPointSize(10);

    std::vector<Point> points = {};

    AddPoint(points, -0.9, -0.9);
    AddPoint(points, -0.7, -0.9);
    AddPoint(points, -0.9, -0.7);
    AddPoint(points, -0.7, -0.7);
    AddPoint(points, -0.5, -0.7);
    AddPoint(points, -0.7, -0.5);
    AddPoint(points, -0.5, -0.5);
    AddPoint(points, -0.9, -0.5);
    AddPoint(points, -0.5, -0.9);

    ExtractPolygons(points);

    /*
        pomysł -> jeżeli w środku wielokąta nie ma żadnego innego punktu (w tym na linii boku)
        oraz żadne z dwóch przylegających do siebie boków nie tworzą kąta 180 stopni
        to musimy policzyc srodek ciezkosci tego wielokąta (lub dwoch punktów)
    */

    /*
    PointData testGravityCenter = CalculateCenterOfGravity(points);

    points.push_back({
        testGravityCenter,
        {0, 0, 0}
	});
    */

    const std::vector<Triangle> triangles = CreateTrianglesFromPoints(points);

    GLuint pointsVertexShader = CompileShader("shaders/shader.vert", GL_VERTEX_SHADER);
    GLuint pointsFragmentShader = CompileShader("shaders/shader.frag", GL_FRAGMENT_SHADER);

    GLuint pointsShaderProgram = CreateShaderProgram({pointsVertexShader, pointsFragmentShader});

	glUseProgram(pointsShaderProgram);

    GLuint vbo;

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLuint vao;

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

	InitializePointsAttribPointers();

	const auto trianglesPoints = TransformTrianglesIntoPoints(triangles);


    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_DYNAMIC_DRAW);

        glDrawArrays(GL_POINTS, 0, points.size());

        glBufferData(GL_ARRAY_BUFFER, trianglesPoints.size() * sizeof(Point), trianglesPoints.data(), GL_DYNAMIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, trianglesPoints.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}