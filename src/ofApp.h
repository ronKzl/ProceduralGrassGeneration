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
		void keyPressed(int key);


		ofEasyCam cam;
		Helper toolbox;
		SweptTube stem; // main stem
		// potentially a vector of stems that will corellate to this one here
		std::vector<SweptTube> branches;
		std::vector<Leaf> leaves;
		ofLight light;

		bool showColor = false;
		bool showAxis = true;
		
};
