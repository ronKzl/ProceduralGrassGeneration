#pragma once
#include "ofMain.h"

/*
Class representing a leaf mesh of the grass
*/
class Leaf {
public:
    void setCenterline(const std::vector<glm::vec3>& pts);
    void setMaxWidth(float w);    
    void setOutwardDirection(glm::vec3 direction);
    void build();                       

    const ofMesh& getMesh() const { return mesh; }

private:
    std::vector<glm::vec3> centerline;
    float maxWidth = 25.0f;             
    glm::vec3 outwardDir;
    ofMesh mesh;
};