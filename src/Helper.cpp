#include "Helper.h"
#include <cmath>

// return a point in 3D on 3-point or 4-point bezier curve
glm::vec3 Helper::bezierPoint(float t, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, bool fourPointCalc) {
    float u = 1.0f - t;
    if (p3 != nullptr && fourPointCalc) {
        return (pow(u, 3) * *p0) + (3 * pow(u, 2) * t * *p1) + (3 * u * pow(t, 2) * *p2) + (pow(t, 3) * *p3); // 4 point bezier formula for a point
    }
    return (pow(u, 2) * *p0) + (2 * u * t * *p1) + (pow(t, 2) * *p2); // 3 point bezier formula for a point
}
// gets x number of concrete points on a arbitrary bezier line defined between p0 - p1 - p2 - p3
void Helper::getBezierLine(int segments, std::vector<glm::vec3>* pts, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, bool fourPointCalc) {
    for (int i = 0; i <= segments; i++) {
        float t = float(i) / float(segments);
        glm::vec3 pos = bezierPoint(t, p0, p1, p2, p3, fourPointCalc);
        pts->push_back(pos);
       
    }
}

// helper to get random vector in the direction of X,Z in which blade of grass will lean towards
glm::vec3 Helper::randomDirectionXZ() {
    glm::vec3 v(ofRandomf(), 0.f, ofRandomf());
    return glm::length2(v) > VECTOR_LENGTH_CUTOFF_POINT ? glm::normalize(v) : glm::vec3(1, 0, 0);
}


// gemerates the 3 vectors needed for a 3 point bezier based on a bit of randomness
void Helper::generateBezier3PointVectors(const glm::vec3& base,
    glm::vec3& p0, glm::vec3& p1, glm::vec3& p2)
{
    float lengthY = ofRandom(GRASS_SPINE.minLength, GRASS_SPINE.maxLength);
    glm::vec3 lean = randomDirectionXZ();
    float leanFrac = ofRandom(0.25f, GRASS_SPINE.maxLean);
    float curveFrac = ofRandom(0.35f, GRASS_SPINE.maxCurve);
    float droopFrac = ofRandom(0.0f, GRASS_SPINE.maxDroop);

    p0 = base;
    p1 = base + glm::vec3(0, lengthY * ofRandom(0.55f, 0.70f), 0) + lean * (leanFrac * lengthY * curveFrac);  
    p2 = base + glm::vec3(0, lengthY * (1.0f - droopFrac), 0) + lean * (leanFrac * lengthY);               
}
// gemerates the 4 vectors needed for a 4 point bezier based on a bit of randomness
void Helper::generateBezier4PointVectors(const glm::vec3& base,
    glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3)
{
    float lengthY = ofRandom(GRASS_SPINE.minLength, GRASS_SPINE.maxLength);
    glm::vec3 lean = randomDirectionXZ();
    float leanFrac = ofRandom(0.25f, GRASS_SPINE.maxLean);
    float curveFrac = ofRandom(0.35f, GRASS_SPINE.maxCurve);
    float droopFrac = ofRandom(0.0f, GRASS_SPINE.maxDroop);

    p0 = base;
    p1 = base + glm::vec3(0, lengthY * ofRandom(0.25f, 0.40f), 0) + lean * (leanFrac * lengthY * 0.10f);       
    p2 = base + glm::vec3(0, lengthY * ofRandom(0.65f, 0.85f), 0) + lean * (leanFrac * lengthY * ofRandom(0.45f, 0.75f) * curveFrac);
    p3 = base + glm::vec3(0, lengthY * (1.0f - droopFrac), 0) + lean * (leanFrac * lengthY);
}

