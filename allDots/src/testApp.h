#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);

		ofxKinect kinect;

		ofxCvGrayscaleImage grayImage;

		ofxCvContourFinder contourFinder;
		
		int nearThreshold;
		int farThreshold;
};
