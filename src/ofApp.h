#pragma once

#include "ofMain.h"
#include "Helper.h"
#include "SweptTube.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);


		ofEasyCam cam;
		Helper toolbox;
		SweptTube stem; // main stem
		// potentially a vector of stems that will corellate to this one here
		SweptTube branch;
		
};
