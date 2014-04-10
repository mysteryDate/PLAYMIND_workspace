#pragma once

#include "ofMain.h"
#include "ofxKinect.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		
		ofxKinect kinect;

		ofImage img;

		int frame;
		bool bSaveSequence;
		
};
