#pragma once

#define DISPLAY_WIDTH 640	
#define DISPLAY_HEIGHT 480

// 585 x 425

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
//#include "armBlob.h"
#include "ArmContourFinder.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void drawLabels();
		void drawEnds();

		ofVideoPlayer	movie;
		bool			bPlaying;

		int				nearThreshold;
		int				farThreshold;
		
		ofxCvColorImage 		colorImg;
		ofxCvGrayscaleImage		grayImg;
		ArmContourFinder	contourFinder;

		ofxCv::ContourFinder 	hands;
		ofxCv::RectTracker			tracker;
		ofxCv::FlowPyrLK		flow;

		//Maximum points of blobs (for testing)
		int xMin;
		int yMin;
		int xMax;
		int yMax;

		//Tracked features
		vector<ofPoint> features;
		
};
