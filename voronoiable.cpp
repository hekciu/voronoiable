#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <limits>
#include <optional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>


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


struct TriangleData {
    PointData pd1;
    PointData pd2;
    PointData pd3;
};


struct Triangle {
    TriangleData triangleData;
    Color color;
};


struct LineEq {
    GLfloat a;
    GLfloat b;
};


bool FloatsEqual(const GLfloat & f1, const GLfloat & f2) {
    const auto constexpr epsilon = std::numeric_limits<GLfloat>().epsilon();

    return std::fabs(f1 - f2) < epsilon;
}


bool FloatsBiggerOrEqual(const GLfloat & f1, const GLfloat & f2) {
    return f1 > f2 || FloatsEqual(f1, f2);
}


bool FloatsLessOrEqual(const GLfloat & f1, const GLfloat & f2) {
    return f1 < f2 || FloatsEqual(f1, f2);
}


bool PointsDataEqual(const PointData & pd1, const PointData & pd2) {
    return FloatsEqual(pd1.x, pd2.x) && FloatsEqual(pd1.y, pd2.y);
}


GLfloat CalculateDistance(const PointData& pd1, const PointData& pd2) {
    return std::sqrt(std::pow(pd1.x - pd2.x, 2) + std::pow(pd1.y - pd2.y, 2));
}


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


GLfloat CalculatePointToLineDistance(const PointData & pointData, const LineEq & lineEquation) {
    LineEq perpendicularLine = {};

    perpendicularLine.a = -lineEquation.a;
    perpendicularLine.b = pointData.y + pointData.x * lineEquation.a;

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


bool IsPointInsideTriangle(const TriangleData& triangleData, const PointData& pointData) {
    GLfloat wholeArea = GetTriangleArea(triangleData.pd1, triangleData.pd2, triangleData.pd3);

    GLfloat firstArea = GetTriangleArea(pointData, triangleData.pd2, triangleData.pd3);
    GLfloat secondArea = GetTriangleArea(triangleData.pd1, pointData, triangleData.pd3);
    GLfloat thirdArea = GetTriangleArea(triangleData.pd1, triangleData.pd2, pointData);

    bool doesPointLayOnTheEdge = FloatsEqual(firstArea, 0) || FloatsEqual(secondArea, 0) || FloatsEqual(thirdArea, 0);

    return !doesPointLayOnTheEdge && FloatsEqual(wholeArea, firstArea + secondArea + thirdArea);
}
 

void PrintTriangles(const std::vector<TriangleData>& triangles) {
    for (const auto& triangle : triangles) {
        std::cout << "triangle" << std::endl;

		std::cout << "p1 x: " << triangle.pd1.x << " y: " << triangle.pd1.y << " | ";
		std::cout << "p2 x: " << triangle.pd2.x << " y: " << triangle.pd2.y << " | ";
		std::cout << "p3 x: " << triangle.pd3.x << " y: " << triangle.pd3.y << " | ";

        std::cout << "" << std::endl;
    }
}


bool DoLinesIntersect(
    const PointData& l1p1,
    const PointData& l1p2,
    const PointData& l2p1,
    const PointData& l2p2
) {
    LineEq lineEq1 = GetLineEquation(l1p1, l1p2);
    LineEq lineEq2 = GetLineEquation(l2p1, l2p2);

    if (FloatsEqual(lineEq1.a, lineEq2.a)) {
        if (FloatsEqual(lineEq1.b, lineEq2.b)) {
            return true;
        }
        else {
            return false;
        }
    }

    // TODO:: use std::pair to handle error if lines do not intersect
    PointData intersectionPoint = GetIntersectionPoint(lineEq1, lineEq2);

    GLfloat x1_first = std::fmin(l1p1.x, l1p2.x);
    GLfloat x1_second = std::fmax(l1p1.x, l1p2.x);

    GLfloat x2_first = std::fmin(l2p1.x, l2p2.x);
    GLfloat x2_second = std::fmax(l2p1.x, l2p2.x);

    bool isOnTheFirstLine = FloatsBiggerOrEqual(intersectionPoint.x, x1_first) && FloatsLessOrEqual(intersectionPoint.x, x1_second);
    bool isOnTheSecondLine = FloatsBiggerOrEqual(intersectionPoint.x, x2_first) && FloatsLessOrEqual(intersectionPoint.x, x2_second);

    bool isOneOfThePoints = PointsDataEqual(intersectionPoint, l1p1) ||
        PointsDataEqual(intersectionPoint, l1p2) ||
        PointsDataEqual(intersectionPoint, l2p1) ||
        PointsDataEqual(intersectionPoint, l2p2);

    // if the intersection point is one of the input points, they DO NOT intersect

    return  isOnTheFirstLine && isOnTheSecondLine && !isOneOfThePoints;
}


bool DoTrianglesIntersect(const TriangleData& t1, const TriangleData& t2) {
    const std::vector<PointData> points1 = {t1.pd1, t1.pd2, t1.pd3};
    const std::vector<PointData> points2 = {t2.pd1, t2.pd2, t2.pd3};

    for (const auto& point : points1) {
        if (IsPointInsideTriangle(t2, point)) return true;
    }

    for (const auto& point : points2) {
        if (IsPointInsideTriangle(t1, point)) return true;
    }

    const std::vector<std::pair<PointData, PointData>> linesT1 = {
        {t1.pd1, t1.pd2},
        {t1.pd2, t1.pd3},
        {t1.pd3, t1.pd1}
    };

    const std::vector<std::pair<PointData, PointData>> linesT2 = {
        {t2.pd1, t2.pd2},
        {t2.pd2, t2.pd3},
        {t2.pd3, t2.pd1}
    };

    uint8_t linesIntersecting = 0;

    for (const auto& line1 : linesT1) {
        for (const auto& line2 : linesT2) {
            if (DoLinesIntersect(line1.first, line1.second, line2.first, line1.second)) {
                linesIntersecting++;
            }
        }
    }

    return linesIntersecting >= 2;
}
std::optional<TriangleData> FindBestTriangle(
    const PointData& point,
    const std::vector<PointData>& points,
    const std::vector<TriangleData>& currentTriangles
) {
    GLfloat initialArea = 2 * 2;
    GLfloat currentBestArea = initialArea;

    std::pair<PointData, PointData> currentBest = {};

    for (size_t i = 0; i < points.size(); i++) {

        size_t point_1_index = i;
        size_t point_2_index = i == points.size() - 1 ? 0 : i + 1;

        const PointData& currentP1 = points[point_1_index];
        const PointData& currentP2 = points[point_2_index];

        bool intersect = false;

        for (const auto& triangle : currentTriangles) {
            if (DoTrianglesIntersect(triangle, { point, currentP1, currentP2 })) {
                intersect = true;
            }
        }

        if (intersect) continue;

        GLfloat area = GetTriangleArea(point, currentP1, currentP2);

        if (area < currentBestArea) {
            currentBestArea = area;
            currentBest.first = currentP1;
            currentBest.second = currentP2;
        }
    }

    if (!FloatsEqual(currentBestArea, initialArea)) {
        return std::make_optional<TriangleData>({
            point,
            currentBest.first,
            currentBest.second
        });
    }

    return std::nullopt;
}


std::vector<TriangleData> ExtractTriangles(const std::vector<Point> & points) {
	std::vector<Point> pointsCopy(points);

    // easier version - triangles

    // for every point find best two points to create triangle

    std::vector<PointData> pointsData = {};

    for (const Point& point : pointsCopy) {
        pointsData.push_back(point.pointData);
    }

    std::vector<TriangleData> triangles = {};

    std::optional<TriangleData> triangle = {};

    for (const auto& point : pointsData) {
        std::cout << "point " << point.x << " " << point.y << '\n';
    }

    for (size_t i = 0; i < pointsData.size(); i++) {
        const auto& point = pointsData[i];

		std::vector<PointData> otherPoints = pointsData;
		otherPoints.erase(otherPoints.begin() + i);

        do {
			triangle = FindBestTriangle(point, otherPoints, triangles);

            if (triangle.has_value() > 0) {
                triangles.push_back(triangle.value());

            }
        } while (triangle.has_value() > 0);
    }

    return triangles;
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

        triangle1.triangleData.pd1.x = point.pointData.x - 0.05;
        triangle1.triangleData.pd1.y = point.pointData.y;
        triangle1.triangleData.pd2.x = point.pointData.x - 0.1;
        triangle1.triangleData.pd2.y = point.pointData.y;
        triangle1.triangleData.pd3.x = point.pointData.x - 0.075;
        triangle1.triangleData.pd3.y = point.pointData.y - 0.05;

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

        point1.pointData = triangle.triangleData.pd1;
        point1.color = triangle.color;

        point2.pointData = triangle.triangleData.pd2;
        point2.color = triangle.color;

        point3.pointData = triangle.triangleData.pd3;
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
    //AddPoint(points, -0.7, -0.9);
    //AddPoint(points, -0.9, -0.7);
    AddPoint(points, -0.7, -0.7);
    //AddPoint(points, -0.5, -0.7);
    //AddPoint(points, -0.7, -0.5);
    AddPoint(points, -0.6, -0.5);
    //AddPoint(points, -0.9, -0.5);
    AddPoint(points, -0.5, -0.8);

    const auto polygons = ExtractTriangles(points);

    std::cout << polygons.size() << '\n';

    PrintTriangles(polygons);

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