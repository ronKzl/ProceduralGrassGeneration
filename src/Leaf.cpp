#include "Leaf.h"
#include "Constants.h"

/*
set the center line the leaf will follow
*/
void Leaf::setCenterline(const std::vector<glm::vec3>& pts) {
    centerline = pts;
}

/*
set how wide the leaf can get at its thickest point
*/
void Leaf::setMaxWidth(float w) {
    maxWidth = std::max(0.0f, w);
}

/*
direction the leaf should stick out from the stem
*/
void Leaf::setOutwardDirection(glm::vec3 direction) {
    outwardDir = direction;
}

/*
build the leaf mesh along the centerline,
for each point on the line offset left and right using a side vector
then connect those points into triangles for the final surface.
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
        float width = maxWidth * sinf(PI * t); // 0 -> max -> 0 along the leaf
        
        // make a green -> yellow gradient
        // init colors each time since lerp modifies the color in place
        ofColor GREEN(50, 120, 50);  
        ofColor YELLOW(180, 180, 80);  
        ofColor leafColor = GREEN.lerp(YELLOW, t);

        glm::vec3 left = centerline[i] - side * (width * 0.5f);
        glm::vec3 right = centerline[i] + side * (width * 0.5f);

        mesh.addVertex(left);
        mesh.addNormal(normal);
        mesh.addColor(leafColor);

        mesh.addVertex(right);
        mesh.addNormal(normal);
        mesh.addColor(leafColor);
    }

    // link each pair of left/right verts to the next pair
    // connect triangles as one strip
    for (int i = 0; i < n - 1; ++i) {
        ofIndexType a = i * 2; // cur left
        ofIndexType b = i * 2 + 1; // cur right
        ofIndexType c = (i + 1) * 2; // next left
        ofIndexType d = (i + 1) * 2 + 1; // next right

        mesh.addIndex(a);
        mesh.addIndex(c);
        mesh.addIndex(b);

        mesh.addIndex(b);
        mesh.addIndex(c);
        mesh.addIndex(d);
    }

}