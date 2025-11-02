#include "ofApp.h"

//Using a swept tube technique to build from a center line that is generating using a bezier curve
//RNG so far -> p1,p2,p3 vectors, set the curve, and we can vary the thickness at the bottom to top - stem
//Can we repeat and make the leafs that will come out also like this but smaller

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(25);
    ofEnableDepthTest();
    ofSetFrameRate(60);
    
    std::vector<glm::vec3> centerline;
    glm::vec3 p0(0, 0, 0); // base
    glm::vec3 p1(50, 70, 55); // rng this
    glm::vec3 p2(-15, 100, -10); // rng this // glm::vec3 p2(-15, 140, -10);
    glm::vec3 p3(0, 0, 0); // rng this

    // actually just rework it to bezier 3points
    toolbox.getBezierLine(100, &p0, &p1, &p2, &p3 ,&centerline); //10 here must correspond to 10 in set resolution
    stem.setCenterline(centerline);     
    stem.setRadius(10.0f, 1.0f);       // base -> tip
    // tube segments on the sides (stop sign 5 increase to do circle),  amount of circles going up
    stem.setResolution(20, 100);       
    stem.build();
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
    ofSetColor(255, 150);
   
    stem.getMesh().drawWireframe();      // wireframe overlay

    cam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
