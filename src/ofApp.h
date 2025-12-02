#pragma once

#include "ofMain.h"
#include "Helper.h"
#include "SweptTube.h"
#include "Leaf.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void draw();
		void generateLeaves();
		void generateStem();
		void exportMesh();
		void generateTopBranches();
		void generateSeeds();
		void keyPressed(int key);
		void glueToOneMesh();
		void generateField();

		ofEasyCam cam;
		Helper toolbox;
		ofLight light;

		// base plant pieces
		SweptTube stem; 
		std::vector<SweptTube> branches;
		std::vector<Leaf> leaves;
		std::vector<ofSpherePrimitive> seeds;
		
		
		bool showColor = true;
		bool showAxis = true;
		bool showField = false;

		// final meshes
		ofMesh grassMesh; // single plant mesh
		ofMesh grassField; // mesh for an entire field of the one plant    
		
};
