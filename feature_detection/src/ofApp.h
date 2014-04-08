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

		void keyPressed(int key);

		ofxKinect kinect;

		ofxCvColorImage colorImg;
        ofxCvGrayscaleImage background;

		ofxCvGrayscaleImage grayImage; // grayscale depth image
        ofxCvGrayscaleImage grayThreshNear;
        ofxCvGrayscaleImage grayThreshFar;
		
		ofxCvContourFinder contourFinder;

		// The size of the output in pixels
		const int displayWidth = 640;
		const int displayHeight = 480;

		int width;
		int height;

        // The centroid of every blob (I don't know if explicity setting the size is a good idea)
		ofPoint centroids[20];
        // The entry point of every blob
        ofPoint entries[20];
        ofPoint exits[20];

        //where on the vector we wanna be
        int vpoint;
    
        //Polyline experiment
        ofPolyline polyLine;
    
        vector< ofPoint > edgePoints;

		int nearThreshold;
		int farThreshold;
    
        bool modeSelection;
		
};
