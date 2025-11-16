#include "ofApp.h"

// Had TODO's over the place -> this is now the spot for all general TODOs:
/*
1. Current bottom leaf generation is not procedural, need to implement some randomness to it and 
take into account some characteristics of the stem when generating its leaves, like avoiding big leaves for small stems

2. Look into recursive logic for the top branches of the grass, almost like tree branch like it spawns from each one
and ends in a seedling placement.

3. Look into how to make procedural seedlings, or just approximate by sphere?

4. texture & shading?

5. refactor & cleanup (seperate procedural generation into more functions)

*/


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(25);
    ofEnableDepthTest();
    ofSetFrameRate(60);

    ofSetSmoothLighting(true); // Smooth lighting calculation


    // Initialize a point light just for real time view
    light.setPointLight();
    light.setPosition(500, 500, 500); // Set the light's position
    
    std::cout << SEED << std::endl;
    ofSetRandomSeed(SEED);

    generateStem();
    generateLeaves();

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    ofEnableLighting();  
    light.enable();

    // FOR TESTING ONLY
    if (showAxis) {
        ofSetLineWidth(2);
        ofDrawAxis(1200);
    }
    


    ofSetColor(60);
    // draw solid mesh
    ofSetColor(ofColor::green);  
    if (showColor) {
        stem.getMesh().draw();
        for (Leaf& leaf : leaves) {
            leaf.getMesh().draw();
        }
        
    }
    else {
        ofSetColor(255, 150);

        stem.getMesh().drawWireframe();
        for (Leaf& leaf : leaves) {
            leaf.getMesh().drawWireframe();
        }
    }
    
    light.disable();
    ofDisableLighting();
    cam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    std::cout << key << std::endl;
    if (key == 'c') {
        showColor = !showColor;
        return;
    }
    if (key == 'a') {
        showAxis = !showAxis;
        return;
    }
    
    SEED++;
    //std::cout << SEED << std::endl;
    ofSetRandomSeed(SEED);

    generateStem();
    generateLeaves();

}
/*
Create the mesh for the central plant stem
*/
void ofApp::generateStem() {
    glm::vec3 p0, p1, p2, p3;
    bool fourBezier = false;
    // 49% chance if we do a 3point stem
    if (ofRandom(1.0f) >= 0.51) {
        toolbox.generateBezier3PointVectors(glm::vec3(0, 0, 0), p0, p1, p2);
    }
    else {
        toolbox.generateBezier4PointVectors(glm::vec3(0, 0, 0), p0, p1, p2, p3);
        fourBezier = true;
    }


    std::vector<glm::vec3> centerline;
    toolbox.getBezierLine(100, &centerline, &p0, &p1, &p2, &p3, fourBezier);
    stem.setCenterline(centerline);
    stem.setRadius(10.f, 1.f);
    stem.setResolution(20, centerline.size() - 1);
    stem.build();
}

/*
Create leaf meshes that are placed along the branch stem
*/
void ofApp::generateLeaves() {
    leaves.clear();
    int sampleCount = stem.centerline.size();

                      
    int numOfLeaves = ofRandom(MIN_LEAVES, MAX_LEAVES);
    for (int i = 0; i < numOfLeaves; ++i) {
        float u = ofRandom(0.15f, 0.45f); // leaves generate between 15% - 45% of the total stem height
        int index = glm::clamp(int(u * (sampleCount - 1)), 2, sampleCount - 3);

        // calc a vector pointing in a random direction on the stem cross-sectional plane to place a leaf on
        glm::vec3 position = stem.centerline[index];
        glm::vec3 tangent = stem.T[index];
        glm::vec3 normal = stem.N[index];
        glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));
        normal = glm::normalize(glm::cross(binormal, tangent));

        float phi = ofRandom(0.0f, TWO_PI);
        glm::vec3 ringDir = cosf(phi) * normal + sinf(phi) * binormal;

        float localRadius = glm::mix(stem.baseRadius, stem.tipRadius, glm::clamp(u, 0.0f, 1.0f));
        glm::vec3 basePoint = position + ringDir * (localRadius * 0.3f);

        // length of leaf
        float leafLength = ofRandom(0.55f, 0.85f) * GRASS_SPINE.maxLength;

        // TODO:randomize all of these 
        float startOut = 0.18f * leafLength;
        float startUp = 0.30f * leafLength;
        float lean = 0.75f * leafLength;
        float droop = 0.55f * leafLength;

        glm::vec3 p0 = basePoint;
        glm::vec3 p1 = basePoint + ringDir * startOut + tangent * startUp;
        glm::vec3 p2 = basePoint + ringDir * lean - tangent * droop; 

        std::vector<glm::vec3> leafLine;
        toolbox.getBezierLine(50, &leafLine, &p0, &p1, &p2);

        Leaf leaf;
        leaf.setCenterline(leafLine);
        leaf.setMaxWidth(localRadius * 3.5f); // <-- TODO:randomize
        leaf.setOutwardDirection(ringDir);
        leaf.build();
        leaves.push_back(leaf);
    }
}

