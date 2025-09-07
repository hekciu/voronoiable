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

    return std::fabs(f1 - f2) < epsilon * 3;
}


bool FloatsBiggerOrEqual(const GLfloat & f1, const GLfloat & f2) {
    return f1 > f2 || FloatsEqual(f1, f2);
}


bool FloatsLessOrEqual(const GLfloat & f1, const GLfloat & f2) {
    return f1 < f2 || FloatsEqual(f1, f2);
}


bool PointsDataEqual(const PointData & pd1, const PointData & pd2) {
    /*
    std::cout << "points data equal " << '\n';
    std::cout << "p1 x: " << pd1.x << " y: " << pd1.y << '\n';
    std::cout << "p2 x: " << pd2.x << " y: " << pd2.y << '\n';
    std::cout << "output " << FloatsEqual(pd1.x, pd2.x) << " " << FloatsEqual(pd1.y, pd2.y) << " " << (FloatsEqual(pd1.x, pd2.x) && FloatsEqual(pd1.y, pd2.y)) <<std::endl;
    */

    return FloatsEqual(pd1.x, pd2.x) && FloatsEqual(pd1.y, pd2.y);
}


GLfloat CalculateDistance(const PointData& pd1, const PointData& pd2) {
    return std::sqrtf(std::powf(pd1.x - pd2.x, 2) + std::powf(pd1.y - pd2.y, 2));
}


LineEq GetLineEquation(const PointData & p1, const PointData & p2){
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


LineEq GetPerpendicularLine(const LineEq& lineEq, const PointData& intersectionPoint) {
    LineEq perpendicularLine = {};

    perpendicularLine.a = -1.0f / lineEq.a;
    perpendicularLine.b = intersectionPoint.y - intersectionPoint.x * perpendicularLine.a;

    return perpendicularLine;
}


GLfloat CalculatePointToLineDistance(const PointData & pointData, const LineEq & lineEquation) {
    const auto perpendicularLine = GetPerpendicularLine(lineEquation, pointData);

    const PointData intersectionPoint = GetIntersectionPoint(lineEquation, perpendicularLine);

    return CalculateDistance(pointData, intersectionPoint);
}


GLfloat GetTriangleArea(const PointData & p1, const PointData & p2, const PointData & p3) {
    const LineEq line = GetLineEquation(p1, p2);

    const GLfloat a = CalculateDistance(p1, p2);

    const GLfloat h = CalculatePointToLineDistance(p3, line);

    return (a * h) / 2.0f;
}


PointData CalculateCenterOfGravity(const std::vector<PointData> & points) {
    PointData sum = {0, 0};

    for (const auto& point : points) {
		sum.x += point.x;
		sum.y += point.y;
    }

    return {
        sum.x / (float)points.size(),
        sum.y / (float)points.size(),
    };
}


bool IsPointInsideTriangle(const TriangleData& triangleData, const PointData& pointData) {
    GLfloat wholeArea = GetTriangleArea(triangleData.pd1, triangleData.pd2, triangleData.pd3);

    GLfloat firstArea = GetTriangleArea(pointData, triangleData.pd2, triangleData.pd3);
    GLfloat secondArea = GetTriangleArea(triangleData.pd1, pointData, triangleData.pd3);
    GLfloat thirdArea = GetTriangleArea(triangleData.pd1, triangleData.pd2, pointData);

    bool doesPointLayOnTheEdge = FloatsEqual(firstArea, 0) || FloatsEqual(secondArea, 0) || FloatsEqual(thirdArea, 0);

    bool result = !doesPointLayOnTheEdge && FloatsEqual(wholeArea, firstArea + secondArea + thirdArea);

    return result;
}


bool IsPointInsideTriangleOrOnTheEdge(const TriangleData& triangleData, const PointData& pointData) {
    GLfloat wholeArea = GetTriangleArea(triangleData.pd1, triangleData.pd2, triangleData.pd3);

    GLfloat firstArea = GetTriangleArea(pointData, triangleData.pd2, triangleData.pd3);
    GLfloat secondArea = GetTriangleArea(triangleData.pd1, pointData, triangleData.pd3);
    GLfloat thirdArea = GetTriangleArea(triangleData.pd1, triangleData.pd2, pointData);

    bool result = FloatsEqual(wholeArea, firstArea + secondArea + thirdArea);

    return result;
}
 

void PrintTrianglesData(const std::vector<TriangleData>& triangles) {
    for (const auto& triangle : triangles) {
        std::cout << "triangle" << std::endl;

		std::cout << "p1 x: " << triangle.pd1.x << " y: " << triangle.pd1.y << " | ";
		std::cout << "p2 x: " << triangle.pd2.x << " y: " << triangle.pd2.y << " | ";
		std::cout << "p3 x: " << triangle.pd3.x << " y: " << triangle.pd3.y << " | ";

        std::cout << "" << std::endl;
    }
}
 

void PrintTriangles(const std::vector<Triangle>& triangles) {
    for (const auto& triangle : triangles) {
        std::cout << "triangle" << std::endl;

		std::cout << "p1 x: " << triangle.triangleData.pd1.x << " y: " << triangle.triangleData.pd1.y << " | ";
		std::cout << "p2 x: " << triangle.triangleData.pd2.x << " y: " << triangle.triangleData.pd2.y << " | ";
		std::cout << "p3 x: " << triangle.triangleData.pd3.x << " y: " << triangle.triangleData.pd3.y << " | ";

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

    uint32_t linesIntersecting = 0;

    for (const auto& line1 : linesT1) {
        for (const auto& line2 : linesT2) {
            if (DoLinesIntersect(line1.first, line1.second, line2.first, line1.second)) {
                linesIntersecting++;
            }
        }
    }

    return linesIntersecting >= 3;
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

            if (triangle.has_value()) {
                triangles.push_back(triangle.value());

            }
        } while (triangle.has_value());
    }

    return triangles;
}


PointData GetCenterOfLine(const PointData& p1, const PointData& p2) {
    return { (p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f};
}


std::vector<TriangleData> ExtractSmallerTriangles(const std::vector<TriangleData>& input) {
    std::vector<TriangleData> output = {};

    for (const auto& triangle : input) {
        const auto center = CalculateCenterOfGravity({ triangle.pd1, triangle.pd2, triangle.pd3 });

        const auto center12 = GetCenterOfLine(triangle.pd1, triangle.pd2);
        output.push_back({triangle.pd1, center12, center});
        output.push_back({center12, triangle.pd2, center});

        const auto center23 = GetCenterOfLine(triangle.pd2, triangle.pd3);
        output.push_back({triangle.pd2, center23, center});
        output.push_back({center23, triangle.pd3, center});

        const auto center31 = GetCenterOfLine(triangle.pd3, triangle.pd1);
        output.push_back({triangle.pd3, center31, center});
        output.push_back({center31, triangle.pd1, center});
    }

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


Point GetNearestPoint(const std::vector<Point>& points, const PointData& ref) {
    assert(points.size() > 0);

    Point best = points[0];

    for (const auto& point : points) {
        const auto curDistance = CalculateDistance(point.pointData, ref);
        const auto curBestDistance = CalculateDistance(best.pointData, ref);

        if (FloatsLessOrEqual(curDistance, curBestDistance)) {
            best = point;
        }

    }

    return best;
}


std::vector<Triangle> AddColorsToTriangles(const std::vector<TriangleData>& trianglesData, const std::vector<Point>& points) {
    std::vector<Triangle> output = {};

    for (const auto& triangleData : trianglesData) {
        const auto centerOfGravity = CalculateCenterOfGravity({ triangleData.pd1, triangleData.pd2, triangleData.pd3 });

        const auto point = GetNearestPoint(points, centerOfGravity);

        output.push_back({ triangleData, point.color });
    }

    return output;
}


LineEq GetPerpendicularLineFromCenter(const PointData& firstPoint, const PointData& secondPoint) {
    const auto lineEq = GetLineEquation(firstPoint, secondPoint);

    const PointData middlePoint = GetCenterOfLine(firstPoint, secondPoint);

    return GetPerpendicularLine(lineEq, middlePoint);
}


std::vector<LineEq> GetLinesBetween(const std::vector<Point>& points) {
    std::vector<LineEq> linesBetween = {};

    for (size_t i = 0; i < points.size(); i++) {
        const auto& firstPoint = points[i];

        for (size_t j = i + 1; j < points.size(); j++) {
			const auto& secondPoint = points[j];

            linesBetween.push_back(GetPerpendicularLineFromCenter(firstPoint.pointData, secondPoint.pointData));
        }
    }

    return linesBetween;
}


std::vector<PointData> GetAllCentersOfLines(const std::vector<Point>& points) {
    std::vector<PointData> output = {};

    for (size_t i = 0; i < points.size(); i++) {
        const auto& firstPoint = points[i];

        for (size_t j = 0; j < points.size(); j++) {
            if (j == i) continue;

			const auto& secondPoint = points[j];

            output.push_back(GetCenterOfLine(firstPoint.pointData, secondPoint.pointData));
        }
    }

    return output;
}


std::vector<PointData> GetAllIntersectionPoints(const std::vector<LineEq>& lines) {
    std::vector<PointData> intersectionPoints = {};

    for (size_t i = 0; i < lines.size(); i++) {
        const auto& firstLine = lines[i];

        for (size_t j = i + 1; j < lines.size(); j++) {
			const auto& secondLine = lines[j];

            intersectionPoints.push_back(GetIntersectionPoint(firstLine, secondLine));
        }
    }

    return intersectionPoints;
}


bool CouldVoronoiTriangleBeAdded(
    const TriangleData& triangle,
    const std::vector<Point>& points,
    const std::vector<PointData>& intersectionPoints,
    const std::vector<Triangle>& triangles
) {
    for (const auto& point : points) {
        if (IsPointInsideTriangleOrOnTheEdge(triangle, point.pointData)) return false;
    }

    for (const auto& point : intersectionPoints) {
        if (IsPointInsideTriangleOrOnTheEdge(triangle, point)) return false;
    }

    for (const auto& curTriangle : triangles) {
        if (DoTrianglesIntersect(triangle, curTriangle.triangleData)) return false;
    }

    return true;
}


template <typename T, typename F>
bool DoesVectorContainElement(
    const std::vector<T>& elements,
    const T& element,
    const F&& areElementsEqual
) {
    for (const T& curElement : elements) {
        if (areElementsEqual(curElement, element)) return true;
    }

    return false;
}


std::vector<PointData> FilterBadIntersectionPoints(
    const std::vector<PointData>& intersectionPoints,
    const std::vector<PointData>& points
) {
    std::vector<PointData> output = {};

    for (const auto& intersectionPoint : intersectionPoints) {
        if (
            !DoesVectorContainElement(output, intersectionPoint, PointsDataEqual) &&
            !DoesVectorContainElement(points, intersectionPoint, PointsDataEqual) &&
            FloatsBiggerOrEqual(intersectionPoint.x, -1.0f) &&
            FloatsLessOrEqual(intersectionPoint.x, 1.0f) &&
            FloatsBiggerOrEqual(intersectionPoint.y, -1.0f) &&
            FloatsLessOrEqual(intersectionPoint.y, 1.0f)
        ) {
            output.push_back(intersectionPoint);
        }
    }

    return output;
}


std::vector<PointData> ExtractPointDatas(const std::vector<Point>& points) {
    std::vector<PointData> output = {};

    for (const auto& point : points) {
        output.push_back(point.pointData);
    }

    return output;
}


std::vector<Triangle> ExtractTriangles1(const std::vector<Point>& points) {
    const auto triangles = ExtractTriangles(points);

    std::cout << triangles.size() << '\n';

    //PrintTriangles(triangles);

    const auto smallerTriangles = ExtractSmallerTriangles(triangles);

    std::cout << smallerTriangles.size() << '\n';

    //PrintTrianglesData(smallerTriangles);

    // const std::vector<Triangle> trianglesToDraw = CreateTrianglesFromPoints(points);

    const std::vector<Triangle> trianglesToDraw = AddColorsToTriangles(smallerTriangles, points);

    return trianglesToDraw;
}


std::vector<Triangle> ExtractTriangles2(const std::vector<Point>& points) {
    const auto linesBetween = GetLinesBetween(points);

    std::cout << "lines between" << '\n';

    for (const auto& line : linesBetween) {
        std::cout << "a: " << line.a << " b: " << line.b << '\n';
    }

    const auto allIntersectionPoints = GetAllIntersectionPoints(linesBetween);

    const auto centersOfLines = GetAllCentersOfLines(points);

    const auto pointsData = ExtractPointDatas(points);

    const auto intersectionPoints = FilterBadIntersectionPoints(allIntersectionPoints, pointsData);

    std::cout << "intersection points" << '\n';

    for (const auto& p : intersectionPoints) {
        std::cout << "x: " << p.x << " y: " << p.y << '\n';
    }

    std::vector<PointData> allPoints = {};
    allPoints.insert(allPoints.end(), allIntersectionPoints.begin(), allIntersectionPoints.end());
    allPoints.insert(allPoints.end(), centersOfLines.begin(), centersOfLines.end());

    std::vector<Triangle> triangles = {};

    for (const auto& point : points) {
        for (const auto& intersectionPoint : intersectionPoints) {
			for (const auto& centerPoint : centersOfLines) {
                const Triangle triangle = {point.pointData, intersectionPoint, centerPoint, point.color};

                if (CouldVoronoiTriangleBeAdded(triangle.triangleData, points, allPoints, triangles)) {
                    triangles.push_back(triangle);
                }
			}
        }
    }

    return triangles;
}


std::pair<PointData, PointData> GetSmallerPair(
    const std::pair<PointData, PointData>&& firstPair,
    const std::pair<PointData, PointData>&& secondPair
) {
    const auto firstSize = CalculateDistance(firstPair.first, firstPair.second);
    const auto secondSize = CalculateDistance(secondPair.first, secondPair.second);

    if (firstSize < secondSize) return firstPair;

    return secondPair;
}


std::pair<PointData, PointData> GetLongestLine(
    const std::vector<std::pair<PointData, PointData>>&& lines
) {
    assert(lines.size() > 0);

    GLfloat currentBestDistance = 0.0f;
    auto currentBest = lines[0];

    for (const auto& line : lines) {
        const auto distance = CalculateDistance(line.first, line.second);

        if (distance > currentBestDistance) {
            currentBestDistance = distance;
            currentBest = line;
        }
    }

    return currentBest;
}


std::vector<Triangle> ExtractTriangles3(const std::vector<Point>& points) {
    const auto bigTriangles = ExtractTriangles(points);

    std::vector<TriangleData> trianglesData = {};

    for (const auto& bigTriangle : bigTriangles) {
        const auto p1p2Center = GetCenterOfLine(bigTriangle.pd1, bigTriangle.pd2);
        const auto p2p3Center = GetCenterOfLine(bigTriangle.pd2, bigTriangle.pd3);
        const auto p3p1Center = GetCenterOfLine(bigTriangle.pd3, bigTriangle.pd1);

        trianglesData.push_back({bigTriangle.pd1, p1p2Center, p3p1Center});

        trianglesData.push_back({bigTriangle.pd2, p1p2Center, p2p3Center});

        trianglesData.push_back({bigTriangle.pd3, p2p3Center, p3p1Center});
    }

    const std::vector<Triangle> trianglesToDraw = AddColorsToTriangles(trianglesData, points);

    return trianglesToDraw;
}


std::vector<Triangle> ExtractTriangles4(const std::vector<Point>& points) {
    const auto bigTriangles = ExtractTriangles(points);

    std::vector<TriangleData> trianglesData = {};

    for (const auto& bigTriangle : bigTriangles) {
        const auto p1p2Center = GetCenterOfLine(bigTriangle.pd1, bigTriangle.pd2);
        const auto p2p3Center = GetCenterOfLine(bigTriangle.pd2, bigTriangle.pd3);
        const auto p3p1Center = GetCenterOfLine(bigTriangle.pd3, bigTriangle.pd1);

        const auto p1p2PerpendicularLine = GetPerpendicularLineFromCenter(bigTriangle.pd1, bigTriangle.pd2);
        const auto p2p3PerpendicularLine = GetPerpendicularLineFromCenter(bigTriangle.pd2, bigTriangle.pd3);
        const auto p3p1PerpendicularLine = GetPerpendicularLineFromCenter(bigTriangle.pd3, bigTriangle.pd1);

        const auto p1IntersectionPoint = GetIntersectionPoint(p1p2PerpendicularLine, p3p1PerpendicularLine);
        const auto p2IntersectionPoint = GetIntersectionPoint(p1p2PerpendicularLine, p2p3PerpendicularLine);
        const auto p3IntersectionPoint = GetIntersectionPoint(p2p3PerpendicularLine, p3p1PerpendicularLine);

        std::cout << "first intersection point x: " << p1IntersectionPoint.x << " y: " << p1IntersectionPoint.y << '\n';
        std::cout << "second intersection point x: " << p2IntersectionPoint.x << " y: " << p2IntersectionPoint.y << '\n';
        std::cout << "third intersection point x: " << p3IntersectionPoint.x << " y: " << p3IntersectionPoint.y << '\n';

        /*
        assert(PointsDataEqual(p1IntersectionPoint, p2IntersectionPoint));
        assert(PointsDataEqual(p2IntersectionPoint, p3IntersectionPoint));
        assert(PointsDataEqual(p3IntersectionPoint, p1IntersectionPoint));
        */

        const auto triangleCircleCenter = p1IntersectionPoint;

        // variant 1 -> acute triangle
        // variant 2 -> right triangle triangle
        // variant 3 -> obtuse triangle
        if (IsPointInsideTriangle(bigTriangle, triangleCircleCenter)) {
			trianglesData.push_back({bigTriangle.pd1, p1p2Center, triangleCircleCenter});
			trianglesData.push_back({bigTriangle.pd1, triangleCircleCenter, p3p1Center});

			trianglesData.push_back({bigTriangle.pd2, p1p2Center, triangleCircleCenter});
			trianglesData.push_back({bigTriangle.pd2, triangleCircleCenter, p2p3Center});

			trianglesData.push_back({bigTriangle.pd3, p2p3Center, triangleCircleCenter});
			trianglesData.push_back({bigTriangle.pd3, triangleCircleCenter, p3p1Center});
        }
        else if (IsPointInsideTriangleOrOnTheEdge(bigTriangle, triangleCircleCenter)) {
            const auto longestLine = GetLongestLine({
                {bigTriangle.pd1, bigTriangle.pd2},
                {bigTriangle.pd2, bigTriangle.pd3},
                {bigTriangle.pd3, bigTriangle.pd1}
            });
        }
        else {
            std::cout << "dupadupa dupa dupadupa " << '\n';
        }
    }

    const std::vector<Triangle> trianglesToDraw = AddColorsToTriangles(trianglesData, points);

    return trianglesToDraw;
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
    //AddPoint(points, -0.9, -0.8);
    AddPoint(points, -0.7, -0.7);
    //AddPoint(points, -0.5, -0.7);
    //AddPoint(points, -0.7, -0.5);
    AddPoint(points, -0.6, -0.5);
    //AddPoint(points, -0.9, -0.5);
    AddPoint(points, -0.5, -0.8);

    //const auto trianglesToDraw = ExtractVoronoiTriangles(points);

    const auto trianglesToDraw = ExtractTriangles4(points);

    //PrintTriangles(trianglesToDraw);

    const auto test = CalculateCenterOfGravity({ {3, 5}, {4, 1}, {1, 0} });

    std::cout << "test123 x: " << test.x << " y: " << test.y << '\n';

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

	const auto trianglesPoints = TransformTrianglesIntoPoints(trianglesToDraw);


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