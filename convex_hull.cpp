#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

//compile with g++ convex_hull.cpp -lGL -lGLU -lglut -lm
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Circle {
    float center_x;
    float center_y;
    float radius;

    std::vector<std::pair<float, float>> getPoints() const {
        const int num_points = 36;
        std::vector<std::pair<float, float>> points(num_points);

        for (int i = 0; i < num_points; i++) {
            float angle = i * (2.0 * M_PI / num_points);
            float x = center_x + radius * std::cos(angle);
            float y = center_y + radius * std::sin(angle);
            points[i] = {x, y};
        }

        return points;
    }
};

std::vector<Circle> circles;

float signedTriangleArea(const std::pair<float, float>& p1, const std::pair<float, float>& p2, const std::pair<float, float>& p3) {
    return (p2.first - p1.first) * (p3.second - p1.second) - (p2.second - p1.second) * (p3.first - p1.first);
}

bool isLeft(const std::pair<float, float>& p, const std::pair<float, float>& p1, const std::pair<float, float>& p2) {
    return signedTriangleArea(p1, p2, p) > 0;
}

std::pair<float, float> getLeftmostPoint(const std::vector<std::pair<float, float>>& points) {
    std::pair<float, float> leftmost = points[0];
    for (const auto& point : points) {
        if (point.first < leftmost.first || (point.first == leftmost.first && point.second < leftmost.second)) {
            leftmost = point;
        }
    }
    return leftmost;
}

float getAngle(const std::pair<float, float>& p1, const std::pair<float, float>& p2) {
    return std::atan2(p2.second - p1.second, p2.first - p1.first);
}

// algoritmo Graham
std::vector<std::pair<float, float>> convexHull(const std::vector<std::pair<float, float>>& points) {
    int n = points.size();
    if (n < 3) {
        return points;
    }

    std::vector<std::pair<float, float>> hull;

    std::pair<float, float> leftmost = getLeftmostPoint(points);

    std::vector<std::pair<float, float>> sortedPoints(points);
    std::sort(sortedPoints.begin(), sortedPoints.end(), [&](const std::pair<float, float>& p1, const std::pair<float, float>& p2) {
        float angle1 = getAngle(leftmost, p1);
        float angle2 = getAngle(leftmost, p2);
        return angle1 < angle2;
    });

    hull.push_back(sortedPoints[0]);

    std::pair<float, float> prevPoint = hull[0];
    std::pair<float, float> currPoint = sortedPoints[1];
    hull.push_back(currPoint);

    for (int i = 2; i < n; i++) {
        std::pair<float, float> nextPoint = sortedPoints[i];
        while (hull.size() >= 2 && !isLeft(nextPoint, prevPoint, currPoint)) {
            hull.pop_back();
            currPoint = prevPoint;
            prevPoint = hull[hull.size() - 2];
        }
        hull.push_back(nextPoint);
        prevPoint = currPoint;
        currPoint = nextPoint;
    }

    return hull;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& circle : circles) {
        
        glColor3f(1.0f, 0.0f, 0.0f);
        
        glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i++) {
                float rad = i * (M_PI / 180.0);
                float x = circle.center_x + circle.radius * cos(rad);
                float y = circle.center_y + circle.radius * sin(rad);
                glVertex2f(x, y);
            }
        glEnd();

        std::vector<std::pair<float, float>> circlePoints = circle.getPoints();

        if (circles.size() >= 3) {
            std::vector<std::pair<float, float>> allPoints;
            for (const auto& c : circles) {
                float x = c.center_x;
                float y = c.center_y;
                allPoints.push_back({x, y});
                for (const auto& p : c.getPoints()) {
                    allPoints.push_back(p);
                }
            }

            std::vector<std::pair<float, float>> hull = convexHull(allPoints);

            glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_LINE_LOOP);
            for (const auto& point : hull) {
                glVertex2f(point.first, point.second);
            }
            glEnd();
        }
    }

    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        Circle circle;
        circle.center_x = (x - WINDOW_WIDTH / 2.0f);
        circle.center_y = -(y - WINDOW_HEIGHT / 2.0f);
        circle.radius = rand() % 21 + 10;
        circles.push_back(circle);

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fecho Convexo");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gluOrtho2D(-WINDOW_WIDTH/2, WINDOW_WIDTH/2, -WINDOW_HEIGHT/2, WINDOW_HEIGHT/2);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
