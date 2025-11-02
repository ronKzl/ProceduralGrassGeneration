#include "Helper.h"
#include "Constants.h"
#include <cmath>

glm::vec3 Helper::bezierPoint(float t, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3) {
    float u = 1.0f - t;
   // return u * u * u * *p0 + 3 * u * u * t * *p1 + 3 * u * t * t * *p2 + t * t * t * *p3; //4 point bezier maybe use later for branches
    return (pow(u, 2) * *p0) + (2 * u * t * *p1) + (pow(t, 2) * *p2); // 3 point bezier what I want now for the grass
}

glm::vec3 Helper::bezierTangent(float t) {
    glm::vec3 p0(0, 0, 0);
    glm::vec3 p1(20, 70, 15);
    glm::vec3 p2(-15, 140, -10);
    glm::vec3 p3(0, 200, 5);
    float u = 1.0f - t;
    return -3 * u * u * p0 + 3 * (u * u - 2 * u * t) * p1 + 3 * (2 * u * t - t * t) * p2 + 3 * t * t * p3;
}

void Helper::getBezierLine(int segments, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, std::vector<glm::vec3>* pts) {
    for (int i = 0; i <= segments; i++) {
        float t = float(i) / float(segments);
        glm::vec3 pos = bezierPoint(t, p0, p1, p2, p3);
        pts->push_back(pos);
       
    }
}

glm::vec3 Helper::safeNormalize(const glm::vec3& v) {
    float magnitude = glm::length(v);
    return (magnitude > VECTOR_LENGTH_CUTOFF_POINT) ? (v / magnitude) : glm::vec3(0, 0, 0);
}

