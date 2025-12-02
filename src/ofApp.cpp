#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(25);
    ofEnableDepthTest();
    ofSetFrameRate(60);

    ofSetSmoothLighting(true);

    light.setPointLight();
    light.setPosition(500, 500, 500); 
    
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
    else { // show wireframe
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
    if (key == 'r') { // do another generation based on a different seed
        SEED++;
        std::cout << SEED << std::endl;
        ofSetRandomSeed(SEED);

        generateStem();
        generateLeaves();
        generateTopBranches();
        generateSeeds();
        generateField();
    }
    if (key == 'g') { // show the model as a field/solo
        showField = !showField;
    }
    
}

/*
create the mesh for the central plant stem
picks either a 3 point or 4 point bezier spine and sweeps a tube along it
*/
void ofApp::generateStem() {
    glm::vec3 p0, p1, p2, p3;
    bool fourBezier = false;
    // 49% chance if we do a 3 point stem
    if (ofRandom(1.0f) >= STEM_PARAMS.threePointChance) {
        toolbox.generateBezier3PointVectors(glm::vec3(0, 0, 0), p0, p1, p2);
    }
    else {
        toolbox.generateBezier4PointVectors(glm::vec3(0, 0, 0), p0, p1, p2, p3);
        fourBezier = true;
    }
    // sample the bezier curve into a polyline centerline
    std::vector<glm::vec3> centerline;
    toolbox.getBezierLine(STEM_PARAMS.bezierResolution, &centerline, &p0, &p1, &p2, &p3, fourBezier);
    
    // feed the centerline into the swept tube to build geometry
    stem.setCenterline(centerline);
    stem.setRadius(STEM_PARAMS.baseRadius, STEM_PARAMS.tipRadius);
    stem.setResolution(STEM_PARAMS.radialSegments, centerline.size() - 1);
    stem.build();

}

/*
create leaf meshes and attach them to the stem
*/
void ofApp::generateLeaves() {
    leaves.clear();
    int sampleCount = stem.centerline.size();

                      
    int numOfLeaves = ofRandom(MIN_LEAVES, MAX_LEAVES);
    
    // for each leaf 
    // pick a random point on the lower/mid stem
    // pick a random direction outward from the stem
    // create a small bezier curve for the leaf spine
    // build a flat mesh along that curve 

    for (int i = 0; i < numOfLeaves; ++i) {
        float u = ofRandom(LEAF_PARAMS.minPlacement, LEAF_PARAMS.maxPlacement);  // choose position along the stem between 15% and 45% of the height
        int index = glm::clamp(int(u * (sampleCount - 1)), 2, sampleCount - 3);

        // calc a vector pointing in a random direction on the stem cross-sectional plane to place a leaf on
        glm::vec3 position = stem.centerline[index];
        glm::vec3 tangent = stem.T[index]; // direction along stem
        glm::vec3 normal = stem.N[index]; // first perpendicular dir
        glm::vec3 binormal = stem.B[index]; // second perpendicular dir

        // pick a random angle around the stem
        float phi = ofRandom(0.0f, TWO_PI);
        // direction outward from the stem surface
        glm::vec3 ringDir = std::cos(phi) * normal + std::sin(phi) * binormal;

        float localRadius = glm::mix(stem.baseRadius, stem.tipRadius, glm::clamp(u, 0.0f, 1.0f));
        glm::vec3 basePoint = position + ringDir * (localRadius * LEAF_PARAMS.baseOffset);

        // length of leaf 
        float leafLength = ofRandom(LEAF_PARAMS.minLengthFactor, LEAF_PARAMS.maxLengthFactor) * GRASS_SPINE.maxLength;

        // control point offsets to shape leaf
        float startOut = LEAF_PARAMS.bezierOut * leafLength;
        float startUp = LEAF_PARAMS.bezierUp * leafLength;
        float lean = LEAF_PARAMS.bezierLean * leafLength;
        float droop = LEAF_PARAMS.bezierDroop * leafLength;

        glm::vec3 p0 = basePoint;
        glm::vec3 p1 = basePoint + ringDir * startOut + tangent * startUp;
        glm::vec3 p2 = basePoint + ringDir * lean - tangent * droop; 
        
        // sample bezier for smooth leaf spine
        std::vector<glm::vec3> leafLine;
        toolbox.getBezierLine(LEAF_PARAMS.bezierResolution, &leafLine, &p0, &p1, &p2);

        // build the leaf mesh from the spine
        Leaf leaf;
        leaf.setCenterline(leafLine);
        leaf.setMaxWidth(localRadius * ofRandom(LEAF_PARAMS.widthMinFactor,LEAF_PARAMS.widthMaxFactor));
        leaf.setOutwardDirection(ringDir);
        leaf.build();
        leaves.push_back(leaf);
    }
}

/*
creates the branch like grass mesh segments found on the upper half of the decorative grass
(as close as I have managed to code it up... should be recursive in nature but I struggled to get that approach working
so this is a basic version that looks okay at best)
*/
void ofApp::generateTopBranches() {
    branches.clear();

    // the more branches the denser it looks almost like palm grass
    int numBranches = ofRandom(MIN_BRANCHES, MAX_BRANCHES);
    int sampleCount = stem.centerline.size();

    for (int i = 0; i < numBranches; i++) {
        
        // pick placement along stem using normalized u, like leaves do
        float u = ofRandom(BRANCH_GEN_PARAMS.minPlacement, BRANCH_GEN_PARAMS.maxPlacement);
        int index = glm::clamp(int(u * (sampleCount - 1)), 2, sampleCount - 3);

        // similar to how the leaf are done get the vector info of that point from the stem
        glm::vec3 startPos = stem.centerline[index];
        glm::vec3 tangent = stem.T[index];
        glm::vec3 binormal = stem.B[index];

        // pick a random angle around the stem
        float angle = ofRandom(TWO_PI);

        // calculate a vector pointing out from the stem (in the N/B plane)
        glm::vec3 outward = glm::normalize(std::cos(angle) * tangent + std::sin(angle) * binormal);

        // 0.3 outward vs 1.0 upward makes branches hug the stem slightly and look like its growing out of it
        glm::vec3 direction = glm::normalize(outward * BRANCH_GEN_PARAMS.outwardFactor + tangent * BRANCH_GEN_PARAMS.upwardFactor);

        // build the branch mesh
        float length = ofRandom(BRANCH_GEN_PARAMS.minLength, BRANCH_GEN_PARAMS.maxLength);
        glm::vec3 endPos = startPos + (direction * length);

        SweptTube branch;
        std::vector<glm::vec3> branchPoints = { startPos, endPos };

        branch.setCenterline(branchPoints);
        branch.setRadius(ofRandom(BRANCH_GEN_PARAMS.minRadius, BRANCH_GEN_PARAMS.maxRadius), 0.0f);
        branch.setResolution(BRANCH_GEN_PARAMS.radialResolution, BRANCH_GEN_PARAMS.lengthResolution);
        branch.build();

        branches.push_back(branch);
    }
}
/*
place the seed meshes on the tips of each branch (should be more of a clustered thing but no time to do that)
*/
void ofApp::generateSeeds() {
    seeds.clear();

    // for each branch place a seed
    for (SweptTube& branch : branches) {

        // get tip and prev point (aka the 2 points that make up the branch)
        glm::vec3 tip = branch.centerline.back();
        glm::vec3 prev = branch.centerline[branch.centerline.size() - 2];

        // calc the direction the branch is pointing at the tip
        glm::vec3 direction = glm::normalize(tip - prev);

        //make a "seed" approximating by a sphere here 
        ofSpherePrimitive sphere;
        sphere.setResolution(SEED_PARAMS.resolution);

        // place the sphere at the branch tip in the world
        sphere.setPosition(tip);

        // orient the sphere to face along the branch direction
        sphere.lookAt(tip + direction);

        // give elongated "seed" look
        sphere.setScale(SEED_PARAMS.scaleXY, SEED_PARAMS.scaleXY, ofRandom(SEED_PARAMS.minScaleZ, SEED_PARAMS.maxScaleZ));

        float size = ofRandom(SEED_PARAMS.minRadius, SEED_PARAMS.maxRadius);
        sphere.setRadius(size);

        seeds.push_back(sphere);
    }
}

/*
merge stem, leaves, branches and seeds into a single mesh for drawing / export 
*/
void ofApp::glueToOneMesh() {
    grassMesh.clear();

    grassMesh.append(stem.getMesh());

    for (Leaf& leaf : leaves) {
        grassMesh.append(leaf.getMesh());
    }

    for (SweptTube& branch : branches) {
        grassMesh.append(branch.getMesh());
    }

    for (ofSpherePrimitive& sphere : seeds) {
        // copy sphere mesh and its global orientation out
        ofMesh sMesh = sphere.getMesh();
        glm::mat4 matrix = sphere.getGlobalTransformMatrix();
        // add color to the mesh
        ofColor seedColor(240, 230, 180);
        for (int i = 0; i < sMesh.getNumVertices(); i++) {
            sMesh.addColor(seedColor);
        }
        // apply the sphere transform to its vertices so that its in the proper position in the mesh
        for (glm::vec3& v : sMesh.getVertices()) {
            v = glm::vec3(matrix * glm::vec4(v, 1.0));
        }

        grassMesh.append(sMesh);
    }

}
/*
build a field of plants by copying the base plant mesh at random positions
*/
void ofApp::generateField() {
    glueToOneMesh();
    
    grassField.clear();

    for (int i = 0; i < FIELD_PARAMS.numPlants; i++) {

        float x = ofRandom(-FIELD_PARAMS.range, FIELD_PARAMS.range);
        float z = ofRandom(-FIELD_PARAMS.range, FIELD_PARAMS.range);
        glm::vec3 offset(x, 0, z);

        ofMesh tempMesh = grassMesh;

        for (glm::vec3& v : tempMesh.getVertices()) {
            v += offset;
        }

        grassField.append(tempMesh);
    }
}

/*
export the current single plant mesh to a ply file that can load into blender or maya "for further work or whatever else"
*/
void ofApp::exportMesh() {

    grassMesh.save("grassSample.ply");

    std::cout << "Mesh exported to bin/data/grassSample.ply" << std::endl;
}
