#include "ofApp.h"

// Had TODO's over the place -> this is now the spot for all general TODOs:
/*
1. Current bottom leaf generation is not procedural, need to implement some randomness to it and 
take into account some characteristics of the stem when generating its leaves, like avoiding big leaves for small stems

2. Look into recursive logic for the top branches of the grass, almost like tree branch like it spawns from each one
and ends in a seedling placement. <-- should be some recursion but tricky part on how to rotate and transform the branches
using ofRotate and ofTranform did not really work out as expected

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
    

   
    ofDisableArbTex();

    // Load texture - TODO: find better texture or just do a simple gradient
    bool loaded = grassTexture.load("texture_leaf.jpg");
    if (!loaded) {
        std::cout << "Error: Could not load texture_leaf.jpg! Is it in bin/data?" << std::endl;
    }
    else {
        grassTexture.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    }

    // Initialize a point light just for real time view
    light.setPointLight();
    light.setPosition(500, 500, 500); // Set the light's position
    
    light.setDiffuseColor(ofColor(255, 255, 220)); 
    light.setSpecularColor(ofColor(255, 255, 255));
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

    
    if (showColor) {
        
        if (grassTexture.isAllocated()) {
            grassTexture.bind();
        }
        
        stem.getMesh().draw();
        for (Leaf& leaf : leaves) {
            leaf.getMesh().draw();
        }
        
        if (grassTexture.isAllocated()) {
            grassTexture.unbind();
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
    
    if (key == 'c') { // show mesh/filled
        showColor = !showColor;
       
    }
    if (key == 'a') { // show/unshow axis 
        showAxis = !showAxis;
        
    }
    if (key == 'x') { // export mesh
        exportMesh();
    }
    if (key == 'r') { // go next
        SEED++;
        std::cout << SEED << std::endl;
        ofSetRandomSeed(SEED);

        generateStem();
        generateLeaves();
    }
    
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

        // length of leaf                       ----- should be the stem max length not the constant max length
        float leafLength = ofRandom(0.10f, 0.75f) * GRASS_SPINE.maxLength;

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
        leaf.setMaxWidth(localRadius * ofRandom(1.0f,3.5f)); 
        leaf.setOutwardDirection(ringDir);
        leaf.build();
        leaves.push_back(leaf);
    }
}

/*
Export the procedurally generated grass piece (useful if lets say this one in particular is looking really good)
*/
void ofApp::exportMesh() {
    ofMesh exportMesh;

    exportMesh.append(stem.getMesh());

    for (size_t i = 0; i < leaves.size(); i++) {
        exportMesh.append(leaves[i].getMesh());
    }

    exportMesh.save("grassSample.ply");

    std::cout << "Mesh exported to bin/data/grassSample.ply" << std::endl;
}
