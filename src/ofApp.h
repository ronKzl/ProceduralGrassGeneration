#pragma once

#include "ofMain.h"
#include "Helper.h"
#include "SweptTube.h"
#include "Leaf.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void generateLeaves();
		void generateStem();
		void exportMesh();
		void generateTopBranches();
		void generateSeeds();
		void keyPressed(int key);


		ofEasyCam cam;
		Helper toolbox;
		SweptTube stem; // main stem
		std::vector<SweptTube> branches;
		std::vector<Leaf> leaves;
		std::vector<ofSpherePrimitive> seeds;
		ofLight light;
		ofImage grassTexture;
		bool showColor = false;
		bool showAxis = true;
		
};
