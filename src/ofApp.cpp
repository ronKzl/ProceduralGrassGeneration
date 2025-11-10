#include "ofApp.h"



//General TODO:
// Put branch generation logic into a seperate class
// generating many seeds and clustering them on each one of the branches
// seed RNG 
// combine all the meshes into 1 mesh??
// ----- 15, 16 goal
// writing a raytracer (for triangles? or like the entire mesh not sure how to works find out)
// for this or find some other offline rendering technique, exporting mesh?
// ------ 22, 23


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


    glm::vec3 p0, p1, p2, p3;
    toolbox.generateBezier4PointVectors(glm::vec3(0, 0, 0), p0, p1, p2, p3);

    std::vector<glm::vec3> centerline;
    toolbox.getBezierLine(100, &centerline, &p0, &p1, &p2, &p3, true); 
    stem.setCenterline(centerline);
    stem.setRadius(10.f, 1.f);
    stem.setResolution(20, centerline.size() - 1);
    stem.build();


    // branch logic for a single branch 
    // TODO: random branch number, random spread, random shapes for all of them
    //maybe refactor to be like a factory pattern
    //or just for loop and a branch array
    //also maybe they should be perpendicular currently haking it by having shrinking localRadius in base point by 0.7 so its like in the spine
    //and its like 2 seperate meshes maybe there is an algo to mask 2 meshes into 1? <- last part before using all the info in a ray tracer

    for (int i = 0; i < 6; i++) {


        int sampleCount = stem.centerline.size();
        int index = ofRandom(5, sampleCount - 6);

        // get local frame of that index
        glm::vec3 position = stem.centerline[index];
        glm::vec3 tangent = stem.T[index];
        glm::vec3 normal = stem.N[index];
        glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));
        normal = glm::normalize(glm::cross(binormal, tangent));


        float phi = ofRandom(0.0f, TWO_PI);
        glm::vec3 ringDir = std::cos(phi) * normal + std::sin(phi) * binormal;


        float u = static_cast<float>(index) / static_cast<float>(sampleCount - 1);
        float localRadius = glm::mix(stem.baseRadius, stem.tipRadius, glm::clamp(u, 0.0f, 1.0f));


        glm::vec3 basePoint = position + ringDir * (localRadius * 0.7);


        float branchLength = ofRandom(0.25f, 0.45f) * GRASS_SPINE.maxLength * 0.5f;
        float startOut = 0.12f * branchLength;
        float startUp = 0.45f * branchLength;
        float lean = ofRandom(0.35f, 0.85f) * branchLength;
        float droop = ofRandom(0.00f, 0.10f) * branchLength;

        glm::vec3 p00 = basePoint;
        glm::vec3 p11 = basePoint + ringDir * startOut + tangent * startUp;
        glm::vec3 p22 = basePoint + ringDir * lean + tangent * (branchLength - droop);


        std::vector<glm::vec3> branchLine;
        toolbox.getBezierLine(60, &branchLine, &p00, &p11, &p22);
        SweptTube branch;
        branch.setCenterline(branchLine);
        branch.setRadius(localRadius * 0.35f, localRadius * 0.06f);
        branch.setResolution(14, static_cast<int>(branchLine.size()) - 1);
        branch.build();
        branches.push_back(branch);
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    ofEnableLighting();  // Turn on OpenGL lighting
    light.enable();
    
    ofSetColor(60);
    // draw solid mesh
    ofSetColor(ofColor::green);  
    if (isColor) {
        stem.getMesh().draw();
        for (SweptTube& branch : branches) {
            branch.getMesh().draw();
        }
        
    }
    else {
        ofSetColor(255, 150);

        stem.getMesh().drawWireframe();
        for (SweptTube& branch : branches) {
            branch.getMesh().drawWireframe();;
        }
      
    }
    
    light.disable();     // Disable the light source
    ofDisableLighting();
    cam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c') {
        isColor = !isColor;
        return;
    }
    
    
    SEED++;
    //std::cout << SEED << std::endl;
    ofSetRandomSeed(SEED);

    
    
    glm::vec3 p0, p1, p2, p3;
    bool fourBezier = false;
    // 49-51 if we do a 3point or a 4point bezier spine
    if (ofRandom(1.0f) >= 0.51) {
        toolbox.generateBezier3PointVectors(glm::vec3(0, 0, 0), p0, p1, p2);
        //std::cout << "doing 3 point" << std::endl;
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

    branches.clear();
    //branch logic TODO: put this all into a sperate class
    static std::vector<std::pair<int, float>> taken; // (index, phi)
    taken.clear();
    for (int i = 0; i < 15; i++) {
        int sampleCount = stem.centerline.size();
        // even-ish coverage: stratified, making a golden-angle around ring
        const float uMin = 0.45f, uMax = 0.95f;
        const float golden = glm::pi<float>() * (3.0f - std::sqrt(5.0f)); // 2.39996...

        float uStrata = (i + ofRandom(0.15f, 0.85f)) / 15.0f;         
        float u = glm::mix(uMin, uMax, uStrata);                     
        int index = glm::clamp(int(u * (sampleCount - 1)), 5, sampleCount - 6);

        float phi = std::fmod(i * golden, TWO_PI) + ofRandom(-0.12f, 0.12f); 

        bool ok = true;
        for (auto& p : taken) {
            if (std::abs(p.first - index) < 3) { ok = false; break; }          
            float dPhi = std::abs(std::remainder(phi - p.second, TWO_PI));
            if (dPhi < 0.35f) { ok = false; break; }                             
        }
        if (!ok) { continue; } // just skip
        taken.push_back({ index, phi });

        // local frame at that index
        glm::vec3 position = stem.centerline[index];
        glm::vec3 tangent = stem.T[index];
        glm::vec3 normal = stem.N[index];
        glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));
        normal = glm::normalize(glm::cross(binormal, tangent));

        glm::vec3 ringDir = std::cos(phi) * normal + std::sin(phi) * binormal;

        
        float localRadius = glm::mix(stem.baseRadius, stem.tipRadius, glm::clamp(u, 0.0f, 1.0f));

        // base point just inside the surface for now lol
        glm::vec3 basePoint = position + ringDir * (localRadius * 0.7);


        float tipFactor = 1.0f - u;
        float branchLength = ofRandom(0.25f, 0.45f) * GRASS_SPINE.maxLength * 0.5f * glm::mix(0.6f, 1.0f, tipFactor);
        
        float startOut = 0.12f * branchLength;
        float startUp = 0.45f * branchLength;
        float lean = ofRandom(0.35f, 0.85f) * branchLength;
        float droop = ofRandom(0.00f, 0.10f) * branchLength;

        glm::vec3 p00 = basePoint;
        glm::vec3 p11 = basePoint + ringDir * startOut + tangent * startUp;
        glm::vec3 p22 = basePoint + ringDir * lean + tangent * (branchLength - droop);

        // sample the curve and sweep the branch 
        std::vector<glm::vec3> branchLine;
        toolbox.getBezierLine(60, &branchLine, &p00, &p11, &p22);
        SweptTube branch;
                          
        branch.setCenterline(branchLine);
 
        branch.setRadius(localRadius * glm::mix(0.20f, 0.35f, tipFactor), localRadius * glm::mix(0.04f, 0.06f, tipFactor));
        branch.setResolution(14, static_cast<int>(branchLine.size()) - 1);
        branch.build();
        branches.push_back(branch);
        
    }

}

