#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void calibrateImage(ofxCvGrayscaleImage& image);

		void keyPressed(int key);

		ofxKinect kinect;

		ofxCvColorImage colorImg;

		ofxCvGrayscaleImage grayImage; // grayscale depth image
        ofxCvGrayscaleImage grayThreshNear;
        ofxCvGrayscaleImage grayThreshFar;
		
		ofxCvContourFinder contourFinder;

		// The size of the output in pixels
		const int displayWidth = 640;
		const int displayHeight = 480;

		int width;
		int height;

		// The amount of x and y 'calibration'
		int dx;
		int dy;
		// and zoom 
		float zoom;

		int nearThreshold;
		int farThreshold;
    
        bool bThreshWithOpenCV;
        bool modeSelection;
		
};
