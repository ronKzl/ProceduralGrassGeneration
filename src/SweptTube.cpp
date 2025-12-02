#include "SweptTube.h"
#include "Constants.h"

void SweptTube::setCenterline(std::vector<glm::vec3>& pts) { 
    centerline = pts;
}

void SweptTube::setRadius(float baseRadius, float tipRadius) { 
    this->baseRadius = baseRadius; 
    this->tipRadius = tipRadius; 
}

void SweptTube::setResolution(int radialSegments, int lengthSegments) {
    numberOfRadialSegments = std::max(3, radialSegments);
    numberOfSegments = std::max(1, lengthSegments);
}

/*
helper that maps a (ring, column) pair to a flat vertex index
cols wrap around so col == numberOfRadialSegments is the same as 0
*/
ofIndexType SweptTube::getVertexIndex(int ring, int col) {
    return (ofIndexType)(ring * numberOfRadialSegments + (col % numberOfRadialSegments));
}

/*
build the tube mesh
*/
void SweptTube::build() {
    computeParallelTransportFrames();
    generateRingsAndMesh();         
}

/*
compute parallel transport frames (T N B) along the centerline
based on: https://www.youtube.com/watch?v=5LedteSEgOE
*/
void SweptTube::computeParallelTransportFrames() {
    
    int n = centerline.size();
    
    // init vectors with the size of the centerline to be all 0
    T.assign(n, glm::vec3(0));
    N.assign(n, glm::vec3(0));
    B.assign(n, glm::vec3(0));

    // compute tangent at each one of the points on the centerline that is needed for the paralle transport algo
    for (int i = 0; i < n; ++i) {
        glm::vec3 t;
        // first point do a forward diff (next - curr)
        if (i == 0) { 
            t = centerline[1] - centerline[i]; 
        }
        // last point do a back diff (curr - prev)
        else if (i == n - 1) { 
            t = centerline[i] - centerline[i - 1]; 
        }
        // every other point do a neigbour diff (next - prev)
        else { 
            t = centerline[i + 1] - centerline[i - 1]; 
        }
        // set the tanget at this point to the unit vector of the computed tanget vector
        T[i] = glm::normalize(t); 
    }

    // init the first point for the normal and binormal vectors
    glm::vec3 up(0, 1, 0);
    // if dot product of first tangent point is closer to 1.0 need to use a different up direction to avoid a near parallel
    if (fabs(glm::dot(up, T[0])) > 0.95f) {
        up = glm::vec3(1, 0, 0); 
    } 
    
    // make a local frame:
    // T is known, so cross with up to get a side (B)
    // then cross B with T to get the remaining axis (N)
    // all normalized so the frame is clean
    B[0] = glm::normalize(glm::cross(T[0], up)); 
    N[0] = glm::normalize(glm::cross(B[0], T[0]));

    // now have a full local frame at point 0 so do parallel transport frames 
    for (int i = 1; i < n; ++i) {
        glm::vec3 rotationVector = glm::cross(T[i - 1], T[i]); 
        float cosAngle = glm::clamp(glm::dot(T[i - 1], T[i]), -1.0f, 1.0f);
        float magnitude = glm::length(rotationVector);

        // if the new vector magnitude is almost 0 or very close to it keep previous points position (no change)
        if (magnitude < VECTOR_LENGTH_CUTOFF_POINT) {
            N[i] = N[i - 1];
            B[i] = B[i - 1];
        }
        else { // tangents changed here so need to rotate the frame to follow the bend 
            glm::vec3 normalizedRotationVector = glm::normalize(rotationVector);
            float angle = std::atan2(magnitude, cosAngle);
            
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, normalizedRotationVector);
            N[i] = glm::vec3(R * glm::vec4(N[i - 1], 0.0f));
            B[i] = glm::vec3(R * glm::vec4(B[i - 1], 0.0f));
        }
        
    }
}

/*
build the tube vertices and triangle indices
tube is made of rings placed along the centerline using the frame (T N B)
then connect each ring to the next one (quads -> triangles)
*/
void SweptTube::generateRingsAndMesh() {
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int numberOfRings = numberOfSegments + 1; // if have n segments, need n+1 rings to connect them

    // pre allocate space 
    mesh.getVertices().reserve(numberOfRings * numberOfRadialSegments);
    mesh.getNormals().reserve(numberOfRings * numberOfRadialSegments);
    mesh.getIndices().reserve(numberOfSegments * numberOfRadialSegments * 6); // each quad is 2 triangls so 6 indices

    // create each ring of the tube
    for (int i = 0; i < numberOfRings; ++i) {
        
        // blend the ring radius from the base to the tip of the tube
        float t = (float)i / (float)(numberOfRings - 1);
        float radius = glm::mix(baseRadius, tipRadius, t);

        // do color gradient for the tube, like the leaf init colors each time since lerp modifies the color in place
        ofColor DARK_GREEN(20, 50, 20);   
        ofColor YELLOW_GREEN(160, 200, 60);   
        ofColor stemColor = DARK_GREEN.lerp(YELLOW_GREEN, t);

        // create the ring vertices around the tube
        for (int j = 0; j < numberOfRadialSegments; ++j) {
           
            // put vertices around a circle in the plane (N,B)
            // cos/sin move around the circle, radius pushes outward from centerline
            
            // get an angle around the circle based on current index
            float theta = TWO_PI * (float)j / (float)numberOfRadialSegments;

            // calc ring normal (in the cross-section plane) X + Y
            glm::vec3 normal = std::cos(theta) * N[i] + std::sin(theta) * B[i];

            // use that to get the vertex position
            glm::vec3 pos = centerline[i] + (radius * normal);

            mesh.addVertex(pos);
            mesh.addNormal(glm::normalize(normal));
            mesh.addColor(stemColor);
        }
    }

    // connect each ring to the next one to form the tube surface
    // treat the vertices as a 2d grid: (ring i, column j)
    for (int i = 0; i < numberOfRings - 1; ++i) {
        for (int j = 0; j < numberOfRadialSegments; ++j) {
            ofIndexType a = getVertexIndex(i, j);
            ofIndexType b = getVertexIndex(i, j + 1);
            ofIndexType c = getVertexIndex(i + 1, j);
            ofIndexType d = getVertexIndex(i + 1, j + 1);
           
            // triangle 1: a, c, b
            mesh.addIndex(a);
            mesh.addIndex(c);
            mesh.addIndex(b);

            // triangle 2: b, c, d
            mesh.addIndex(b);
            mesh.addIndex(c);
            mesh.addIndex(d);
        }
    }
}