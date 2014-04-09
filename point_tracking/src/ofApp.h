#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "armBlob.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);

		bool paused;

		ofxKinect kinect;


		ofxCvGrayscaleImage grayImage; // grayscale depth image
		
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
    
        bool modeSelection;
      
      	float simplifyTolerance;

      	vector< ofPolyline > polyLines;
        void simplify(ofxCvBlob &blob);

        vector< armBlob > arms;

		
};
