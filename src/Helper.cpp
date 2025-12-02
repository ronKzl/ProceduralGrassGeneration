#include "Helper.h"
#include <cmath>

/*
returns a point along a bezier curve at time t (0.0–1.0).
can do 3-point and 4-point curves based on the flag.
*/ 
glm::vec3 Helper::bezierPoint(float t, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, bool fourPointCalc) {
    float u = 1.0f - t;
    if (p3 != nullptr && fourPointCalc) {
        return (pow(u, 3) * *p0) + (3 * pow(u, 2) * t * *p1) + (3 * u * pow(t, 2) * *p2) + (pow(t, 3) * *p3); // 4 point bezier formula for a point
    }
    return (pow(u, 2) * *p0) + (2 * u * t * *p1) + (pow(t, 2) * *p2); // 3 point bezier formula for a point
}

/*
get a vector of sample points on a bezier curve from t = 0 to 1.
more segments specified the smoother the curve will be.
*/
void Helper::getBezierLine(int segments, std::vector<glm::vec3>* pts, glm::vec3* p0, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, bool fourPointCalc) {
    for (int i = 0; i <= segments; i++) {
        float t = float(i) / float(segments);
        glm::vec3 pos = bezierPoint(t, p0, p1, p2, p3, fourPointCalc);
        pts->push_back(pos);
       
    }
}

/*
returns a random direction in the horizontal (XZ) plane.
determines where the grass blade will lean left/right.
*/
glm::vec3 Helper::randomDirectionXZ() {
    glm::vec3 v(ofRandomf(), 0.0f, ofRandomf());
    return glm::length2(v) > VECTOR_LENGTH_CUTOFF_POINT ? glm::normalize(v) : glm::vec3(1, 0, 0);
}


/*
creates 3 control points for a grass blade bezier curve.
adds randomized height, lean, curvature, and droop so each blade looks unique.
*/
void Helper::generateBezier3PointVectors(const glm::vec3& base,
    glm::vec3& p0, glm::vec3& p1, glm::vec3& p2)
{
    // get blade height
    float lengthY = ofRandom(GRASS_SPINE.minLength, GRASS_SPINE.maxLength);
    // get direction of blade lean
    glm::vec3 lean = randomDirectionXZ();
    // get how much it will lean, curve and droop
    float leanFrac = ofRandom(GRASS_SPINE.minLean, GRASS_SPINE.maxLean);
    float curveFrac = ofRandom(GRASS_SPINE.minCurve, GRASS_SPINE.maxCurve);
    float droopFrac = ofRandom(GRASS_SPINE.minDroop, GRASS_SPINE.maxDroop);

    p0 = base;
    p1 = base + glm::vec3(0, lengthY * ofRandom(0.55f, 0.70f), 0) + lean * (leanFrac * lengthY * curveFrac);  
    p2 = base + glm::vec3(0, lengthY * (1.0f - droopFrac), 0) + lean * (leanFrac * lengthY);               
}

/*
creates 4 control points for a grass blade bezier curve
same idea as 3 point but with an extra mid point for more shape control
*/
void Helper::generateBezier4PointVectors(const glm::vec3& base,
    glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3)
{
    float lengthY = ofRandom(GRASS_SPINE.minLength, GRASS_SPINE.maxLength);
    glm::vec3 lean = randomDirectionXZ();
    float leanFrac = ofRandom(GRASS_SPINE.minLean, GRASS_SPINE.maxLean);
    float curveFrac = ofRandom(GRASS_SPINE.minCurve, GRASS_SPINE.maxCurve);
    float droopFrac = ofRandom(GRASS_SPINE.minDroop, GRASS_SPINE.maxDroop);

    p0 = base;
    // two mid points for smoother curvature
    p1 = base + glm::vec3(0, lengthY * ofRandom(0.25f, 0.40f), 0) + lean * (leanFrac * lengthY * 0.10f);       
    p2 = base + glm::vec3(0, lengthY * ofRandom(0.65f, 0.85f), 0) + lean * (leanFrac * lengthY * ofRandom(0.45f, 0.75f) * curveFrac);
    p3 = base + glm::vec3(0, lengthY * (1.0f - droopFrac), 0) + lean * (leanFrac * lengthY);
}

