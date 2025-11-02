#include "ofApp.h"



//General TODO:
// generating many branches and placing them along the spine
// branch RNG
// generating many seeds and clustering them on each one of the branches
// seed RNG 
// ------ doable next weekend (8,9)
// combine all the meshes into 1 mesh??
// writing a raytracer for this or some other offline rendering technique that I don't know about yet
//------ (15,16)


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(25);
    ofEnableDepthTest();
    ofSetFrameRate(60);
    
    /*std::vector<glm::vec3> centerline;
    glm::vec3 p0(0, 0, 0); // base
    glm::vec3 p1(50, 70, 55); // rng this
    glm::vec3 p2(-15, 100, -10); // rng this // glm::vec3 p2(-15, 140, -10);
    glm::vec3 p3(0, 0, 0); // rng this

    // actually just rework it to bezier 3points
    toolbox.getBezierLine(100, &centerline, &p0, &p1, &p2); //10 here must correspond to 10 in set resolution
    stem.setCenterline(centerline);     
    stem.setRadius(10.0f, 1.0f);       // base -> tip
    // tube segments on the sides (stop sign 5 increase to do circle),  amount of circles going up
    stem.setResolution(20, 100);       
    stem.build();*/
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

    branch.setCenterline(branchLine);
    branch.setRadius(localRadius * 0.35f, localRadius * 0.06f);     
    branch.setResolution(14, static_cast<int>(branchLine.size()) - 1);
    branch.build();
   
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();

    
    ofSetColor(60);
    // draw solid mesh
    ofSetColor(ofColor::green);  
    stem.getMesh().draw();
    branch.getMesh().draw();
    ofSetColor(255, 150);
   
    stem.getMesh().drawWireframe();     
    branch.getMesh().drawWireframe();

    cam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    SEED++;
    std::cout << SEED << std::endl;
    ofSetRandomSeed(SEED);

    
    
    glm::vec3 p0, p1, p2, p3;
    bool fourBezier = false;
    // 49-51 if we do a 3point or a 4point bezier spine
    if (ofRandom(1.0f) >= 0.51) {
        toolbox.generateBezier3PointVectors(glm::vec3(0, 0, 0), p0, p1, p2);
        std::cout << "doing 3 point" << std::endl;
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

    //branch logic
    int sampleCount = stem.centerline.size();
    int index = ofRandom(5, sampleCount - 6);

    // local frame at that index
    glm::vec3 position = stem.centerline[index];
    glm::vec3 tangent = stem.T[index];
    glm::vec3 normal = stem.N[index];
    glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));
    normal = glm::normalize(glm::cross(binormal, tangent));

    // choose azimuth around the ring
    float phi = ofRandom(0.0f, TWO_PI);
    glm::vec3 ringDir = std::cos(phi) * normal + std::sin(phi) * binormal;

    // compiler keeps screaming so just casting it for now TODO FIX LATER
    float u = static_cast<float>(index) / static_cast<float>(sampleCount - 1);
    float localRadius = glm::mix(stem.baseRadius, stem.tipRadius, glm::clamp(u, 0.0f, 1.0f));

    // base point just inside the surface for now lol
    glm::vec3 basePoint = position + ringDir * (localRadius * 0.7);

    
    float branchLength = ofRandom(0.25f, 0.45f) * GRASS_SPINE.maxLength * 0.5f;
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

    branch.setCenterline(branchLine);
    branch.setRadius(localRadius * 0.35f, localRadius * 0.06f);
    branch.setResolution(14, static_cast<int>(branchLine.size()) - 1);
    branch.build();

}

