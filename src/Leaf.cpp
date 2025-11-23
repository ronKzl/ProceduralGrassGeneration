#include "Leaf.h"
#include "Constants.h"

void Leaf::setCenterline(const std::vector<glm::vec3>& pts) {
    centerline = pts;
}

void Leaf::setMaxWidth(float w) {
    maxWidth = std::max(0.0f, w);
}

void Leaf::setOutwardDirection(glm::vec3 direction) {
    outwardDir = direction;
}

/*
Build the mesh of a leaf, its defined as a collection of points following the centerline.
Basically just calculate and offset points to the left and right of the line depending on the perpendciular vector,
then form triangles from them.
*/
void Leaf::build() {
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int n = centerline.size();
 
    // calculate a new vector that is perpendicular to both outwardDir and the Y axis
    glm::vec3 side = glm::normalize(glm::cross(outwardDir,glm::vec3(0,1,0)));
    
    // pick some normal just for lighting (perpendicular to side and world Y)
    glm::vec3 normal = glm::normalize(glm::cross(side, glm::vec3(1, 0, 0)));
    
    mesh.getVertices().reserve(n * 2);
    mesh.getNormals().reserve(n * 2);
    mesh.getTexCoords().reserve(n * 2);

    for (int i = 0; i < n; ++i) {
        float t = (float)i / (float)(n - 1); // goes from 0 to 1
        float width = maxWidth * sinf(PI * t); // calculate width of leaf as it goes along 0 -> max -> 0
        
        ofColor GREEN(50, 120, 50);   // Standard Green
        ofColor YELLOW(180, 180, 80);   // Dry Yellowish Tip
        ofColor leafColor = GREEN.lerp(YELLOW, t);

        glm::vec3 left = centerline[i] - side * (width * 0.5f);
        glm::vec3 right = centerline[i] + side * (width * 0.5f);

        mesh.addVertex(left);
        mesh.addNormal(normal);
        mesh.addTexCoord(glm::vec2(0.0f, t));
        mesh.addColor(leafColor);

        mesh.addVertex(right);
        mesh.addNormal(normal);
        mesh.addTexCoord(glm::vec2(1.0f, t));
        mesh.addColor(leafColor);
    }

    // connect triangles as one strip
    for (int i = 0; i < n - 1; ++i) {
        ofIndexType a = i * 2;
        ofIndexType b = i * 2 + 1;
        ofIndexType c = (i + 1) * 2;
        ofIndexType d = (i + 1) * 2 + 1;

        mesh.addIndex(a);
        mesh.addIndex(c);
        mesh.addIndex(b);

        mesh.addIndex(b);
        mesh.addIndex(c);
        mesh.addIndex(d);
    }

}