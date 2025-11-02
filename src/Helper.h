#pragma once
#include "ofMain.h"
#include "Constants.h"




/*
A toolbox class that holds all the math and RNG functions needed to create procedural spines, branches and seeds
    - Bezier curve calculation (3 points and in future 4 points)
    - TODO: rng functions for the vector points and size and radius
    - TODO: other helpful functions
    */
class Helper {
public:
    void getBezierLine(int segments, std::vector<glm::vec3>* pts, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, 
        glm::vec3* p3 = nullptr, bool fourPointCalc = false);
    
    
    void generateBezier3PointVectors(const glm::vec3& base,
        glm::vec3& p0, glm::vec3& p1, glm::vec3& p2);

    
    void generateBezier4PointVectors(const glm::vec3& base,
        glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3);
    
private:
    glm::vec3 bezierPoint(float t, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, bool fourPointCalc);
    glm::vec3 randomDirectionXZ();

};

    
