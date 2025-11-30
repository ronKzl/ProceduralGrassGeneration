#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(25);
    ofEnableDepthTest();
    ofSetFrameRate(60);

    ofSetSmoothLighting(true); // Smooth lighting calculation

    ofDisableArbTex();

    // Initialize a point light just for real time view
    light.setPointLight();
    light.setPosition(500, 500, 500); // Set the light's position
    
    light.setDiffuseColor(ofColor(255, 255, 220)); 
    light.setSpecularColor(ofColor(255, 255, 255));
    std::cout << SEED << std::endl;
    ofSetRandomSeed(SEED);

    generateStem();
    generateLeaves();
    generateTopBranches();
    generateSeeds();
    generateField();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    ofEnableLighting();
    light.enable();

    if (showAxis) {
        ofSetLineWidth(2);
        ofDrawAxis(1200);
    }

    if (showColor) {
        if (showField) {
            grassField.draw();
        }
        else {
            grassMesh.draw();
        }
    }
    else {
        // WIREFRAME MODE
        ofSetColor(255, 150);
        if (showField) {
            grassField.drawWireframe();
        }
        else {
            grassMesh.drawWireframe();
        }
        
    }

    light.disable();
    ofDisableLighting();
    cam.end();

    ofDisableDepthTest();
    ofSetColor(255);

    std::string msg = "CONTROLS:\n";
    msg += "-------------------\n";
    msg += "[G] : Toggle Field Mode (Sea of Grass)\n";
    msg += "[R] : Regenerate Single Plant\n";
    msg += "[C] : Toggle Color / Wireframe\n";
    msg += "[A] : Toggle Axis\n";
    msg += "[X] : Export Mesh\n";
    msg += "\nFPS: " + ofToString(ofGetFrameRate());

    ofDrawBitmapString(msg, 20, 20);
    ofEnableDepthTest();

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
        generateTopBranches();
        generateSeeds();
        generateField();
    }
    if (key == 'g') { 
        showField = !showField;
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
        glm::vec3 binormal = stem.B[index];
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
Creates the branch like grass mesh segments found on the upper half of the decorative grass
*/
void ofApp::generateTopBranches() {
    branches.clear();

    // the more branches the denser it looks almost like palm grass
    int numBranches = ofRandom(MIN_BRANCHES, MAX_BRANCHES);

    for (int i = 0; i < numBranches; i++) {
        int n = stem.centerline.size();
        // from like 25% to 90% on the main stem the branches can generate so pick an index along there
        int index = (int)ofRandom(n * 0.25f, n * 0.90f);

        // similar to how the leaf are done get the vector info of that point from the stem
        glm::vec3 startPos = stem.centerline[index];
        glm::vec3 tangent = stem.T[index];
        glm::vec3 binormal = stem.B[index];

        // pick a random angle around the stem
        float angle = ofRandom(TWO_PI);

        // calculate a vector pointing out from the stem
        glm::vec3 outward = glm::normalize(cosf(angle) * tangent + sinf(angle) * binormal);

        // 0.3 outward vs 1.0 upward makes them like go along the stem
        glm::vec3 direction = glm::normalize(outward * 0.3f + tangent * 1.0f);

        // make branches longer to match the pampas reference
        float length = ofRandom(GRASS_BRANCH.minLength, GRASS_BRANCH.maxLength);
        glm::vec3 endPos = startPos + (direction * length);

        SweptTube branch;
        std::vector<glm::vec3> branchPoints = { startPos, endPos };

        branch.setCenterline(branchPoints);
        branch.setRadius(ofRandom(1.0f, 3.0f), 0.0f);
        branch.setResolution(3, 1);
        branch.build();

        branches.push_back(branch);
    }
}
/*
Place the seeds on the branches
*/
void ofApp::generateSeeds() {
    seeds.clear();

    for (auto& branch : branches) {

        // get tip and prev point
        glm::vec3 tip = branch.centerline.back();
        glm::vec3 prev = branch.centerline[branch.centerline.size() - 2];

        // calc the direction the branch is pointing
        glm::vec3 direction = glm::normalize(tip - prev);

        ofSpherePrimitive sphere;
        sphere.setResolution(8);

        sphere.setPosition(tip);

        sphere.lookAt(tip + direction);

        sphere.setScale(0.5f, 0.5f, ofRandom(2.0f,3.0f));

        float size = ofRandom(2.0f, 4.0f);
        sphere.setRadius(size);

        seeds.push_back(sphere);
    }
}


void ofApp::glueToOneMesh() {
    grassMesh.clear();

    grassMesh.append(stem.getMesh());

    for (auto& leaf : leaves) {
        grassMesh.append(leaf.getMesh());
    }

    for (auto& branch : branches) {
        grassMesh.append(branch.getMesh());
    }

    for (auto& sphere : seeds) {
        ofMesh sMesh = sphere.getMesh();
        glm::mat4 matrix = sphere.getGlobalTransformMatrix();

        ofColor seedColor(240, 230, 180);
        for (int i = 0; i < sMesh.getNumVertices(); i++) {
            sMesh.addColor(seedColor);
        }

        for (auto& v : sMesh.getVertices()) {
            v = glm::vec3(matrix * glm::vec4(v, 1.0));
        }

        grassMesh.append(sMesh);
    }

}

void ofApp::generateField() {
    glueToOneMesh();
    
    grassField.clear();

    int numPlants = 250;

    for (int i = 0; i < numPlants; i++) {

        float x = ofRandom(-2000, 2000);
        float z = ofRandom(-2000, 2000);
        glm::vec3 offset(x, 0, z);

        ofMesh tempMesh = grassMesh;

        for (auto& v : tempMesh.getVertices()) {
            v += offset;
        }

        grassField.append(tempMesh);
    }
}

/*
Export the procedurally generated grass piece (useful if lets say this one in particular is looking really good)
*/
void ofApp::exportMesh() {

    grassMesh.save("grassSample.ply");

    std::cout << "Mesh exported to bin/data/grassSample.ply" << std::endl;
}
