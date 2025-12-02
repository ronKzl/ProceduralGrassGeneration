#pragma once
#include "ofMain.h"
#include "Constants.h"

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

    
