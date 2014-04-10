#pragma once

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "armBlob.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);

		ofVideoPlayer	movie;
		bool			bPlaying;

		int				nearThreshold;
		int				farThreshold;

		ofxCvColorImage 		colorImg;
		ofxCvGrayscaleImage		grayImg;
		ofxCv::ContourFinder	contourFinder;
		bool showLabels;

		vector< armBlob > arms;
		float tolerance;
		
};
