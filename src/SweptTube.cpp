#include "SweptTube.h"
#include "Constants.h"

void SweptTube::setCenterline(const std::vector<glm::vec3>& pts) { 
    this->centerline = std::move(pts);
}

void SweptTube::setRadius(float baseRadius, float tipRadius) { 
    this->baseRadius = baseRadius; 
    this->tipRadius = tipRadius; 
}

void SweptTube::setResolution(int radialSegments, int lengthSegments) {
    this->numberOfRadialSegments = std::max(3, radialSegments);
    this->numberOfSegments = std::max(1, lengthSegments);
}
/*
helper that computes a flat vertex index from a 2D grid coordinate (ring, column)
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
https://www.youtube.com/watch?v=5LedteSEgOE parallel transport frames algo for computing a local coordinate frame (T, N, B) at every point
on the centerline

TODO: rework to accept centerline as parameter so I can also use it for the branches
*/
void SweptTube::computeParallelTransportFrames() {
    //init vectors with the size of the centerline to be all 0
    size_t n = centerline.size();
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
        //set the tanget at this point to the unit vector of the computed tanget vector
        T[i] = glm::normalize(t); //safe
    }

    // init the first point for the normal and binormal vectors
    glm::vec3 up(0, 1, 0);
    // if dot product of first tangent point is closer to 1.0 need to use a different up direction to avoid a near parallel
    if (fabs(glm::dot(up, T[0])) > 0.95f) {
        up = glm::vec3(1, 0, 0); 
    } 
    //safe normalize for 2 here
    B[0] = glm::normalize(glm::cross(T[0], up)); //compute a vector perpendicular to both, initial binormal
    N[0] = glm::normalize(glm::cross(B[0], T[0])); //compute the right handed frame(T, N, B) at the start

    // parallel transport frames algo from video
    for (int i = 1; i < n; ++i) {
        glm::vec3 rotationVector = glm::cross(T[i - 1], T[i]); 
        float cosAngle = glm::clamp(glm::dot(T[i - 1], T[i]), -1.0f, 1.0f);
        float magnitude = glm::length(rotationVector);

        // if the new vector magnitude is almost 0 or very close to it keep previous points position
        if (magnitude < VECTOR_LENGTH_CUTOFF_POINT) {
            N[i] = N[i - 1];
            B[i] = B[i - 1];
        }
        else { //cacl
            glm::vec3 normalizedRotationVector = rotationVector / magnitude; // normalize the vector
            float angle = std::atan2(magnitude, cosAngle);
            
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, normalizedRotationVector);
            N[i] = glm::vec3(R * glm::vec4(N[i - 1], 0.0f));
            B[i] = glm::vec3(R * glm::vec4(B[i - 1], 0.0f));
        }
        
        // do I need to normalize again to account for conversion error??? (safe)
        //B[i] = glm::normalize(glm::cross(T[i], N[i]));
        //N[i] = glm::normalize(glm::cross(B[i], T[i]));
    }
}

/*
make quads by connecting vertex j in ring i to vertex j and j+1 in ring i+1
then split each quad into 2 triangles
*/
void SweptTube::generateRingsAndMesh() {
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int numberOfRings = numberOfSegments + 1; //if have n segments, need n+1 rings to connect them

    // pre allocate space -> save time by not reallocating a bigger DS
    mesh.getVertices().reserve(numberOfRings * numberOfRadialSegments);
    mesh.getNormals().reserve(numberOfRings * numberOfRadialSegments);
    mesh.getTexCoords().reserve(numberOfRings * numberOfRadialSegments);
    mesh.getIndices().reserve(numberOfSegments * numberOfRadialSegments * 6); // each quad is 2 triangls so 6 indices

    // create the ring vertices
    for (int i = 0; i < numberOfRings; ++i) { //for the entire tube
        float t = (float)i / (float)(numberOfRings - 1);
        float radius = glm::mix(baseRadius, tipRadius, t);

        for (int j = 0; j < numberOfRadialSegments; ++j) { //for 1 ring in the tube
            
            float theta = TWO_PI * (float)j / (float)numberOfRadialSegments;

            // calc ring normal (in the cross-section plane)
            glm::vec3 n = std::cos(theta) * N[i] + std::sin(theta) * B[i];

            // use that to get the vertex position
            glm::vec3 p = centerline[i] + radius * n;

            // save the computed data
            mesh.addVertex(p);
            mesh.addNormal(glm::normalize(n)); //use later for lighting
            // add UVs: u around, v along, will use later for lighting
            mesh.addTexCoord(glm::vec2((float)j / (float)numberOfRadialSegments, t));
        }
    }

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