#pragma once

// Calibration variables
// -----------------------------------
// Cropping out kinect data
#define KINECT_CROP_LEFT 	28
#define KINECT_CROP_RIGHT 	10
#define KINECT_CROP_TOP 	14
#define KINECT_CROP_BOTTOM 	27

// For the contour finder
#define MIN_CONTOUR_AREA 1000
// -----------------------------------

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();


		void update();
		void transformInput();

		void draw();
		void drawFeedback();

		struct Beaver
		{
			ofVec2f v;
			ofPoint pos;
		};

		// Input Processing
		ofxKinect 			kinect;
		ofVideoPlayer 		video;		
		cv::Mat 			input;
		cv::Mat 			croppedInput;
		ofxCvGrayscaleImage	kinectImg;
		ofxCvGrayscaleImage kinectBackground;
		int 				nearThreshold;
		int 				farThreshold;
		bool 				bLearnBackground;

		ofxCvColorImage 	colorInput;
		ofxCvColorImage 	colorBackground;
		ofxCvGrayscaleImage 	monoColorInput;
		ofxCvGrayscaleImage 	monoColorBackground;
		
		ofxCv::ContourFinder contourFinder;
		ofxCv::ContourFinder contourFinder2;

		void keyPressed(int key);
};
