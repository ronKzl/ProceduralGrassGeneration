#pragma once
#include "ofMain.h"


/*
A toolbox class that holds all the math and RNG functions needed to create procedural spines, branches and seeds
    - Bezier curve calculation (3 points and in future 4 points)
    - TODO: rng functions for the vector points and size and radius
    - TODO: other helpful functions
    */
class Helper {
public:
    void getBezierLine(int segments, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, std::vector<glm::vec3>* pts);
    glm::vec3 safeNormalize(const glm::vec3& v);
private:
    glm::vec3 bezierPoint(float t, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3);
    glm::vec3 bezierTangent(float t);

    
};