#pragma once
#include "ofMain.h"

/*
swept tube mesh used for the grass stem and branches
builds a tube along a centerline using parallel transport frames
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
    void generateRingsAndMesh(); // create vertices, normals and indices
    ofIndexType getVertexIndex(int ring, int col);
    
    std::vector<glm::vec3> centerline; // bezier generated centerline
    std::vector<glm::vec3> T, N, B; // per-point frame vectors (tanget, normal, binormal)

    float baseRadius; // circle radius of the base of the tube                
    float tipRadius;  //circle radius of the tip of the tube                  
    int numberOfRadialSegments; // number of vertices that form each ring
    int numberOfSegments; // number of ring intervals along the centerline

    ofMesh mesh;
};