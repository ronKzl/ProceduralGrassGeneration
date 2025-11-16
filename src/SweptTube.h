#pragma once
#include "ofMain.h"


/*
Swept (Volume) Surface builder for a tube structure,
currently just used to represent the stem of the grass

*/
class SweptTube {
public:
    // sets the centerline along which the sweep will happen
    void setCenterline(std::vector<glm::vec3>& pts);

    // sets tube radius (linear taper from base to tip)
    void setRadius(float baseRadius, float tipRadius);

    // sets the number of vertices around each ring and the number of rings along the Y axis
    void setResolution(int radialSegments, int lengthSegments);

    // builds the mesh
    void build();
    
    // get the mesh for rendering
    const ofMesh& getMesh() const { return mesh; }

    void computeParallelTransportFrames(); // build per-point frames (T, N, B)
    void generateRingsAndMesh(); // create vertices, normals, uvs, indices
    ofIndexType getVertexIndex(int ring, int col);
    
    std::vector<glm::vec3> centerline; // bezier generated centerline
    std::vector<glm::vec3> T, N, B; // per-point frame vectors (tanget, normal, binormal)

    float baseRadius; // circle radius of the base of the tube                
    float tipRadius;  //circle radius of the tip of the tube                  
    int numberOfRadialSegments; // number of vertices that form the ring
    int numberOfSegments; // number of ring intervals

    ofMesh mesh;
};