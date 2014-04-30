#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();


		void update();
		void learnBackground();

		void draw();



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
		ofxCvGrayscaleImage kinectDiff;
		int 				nearThreshold;
		int 				farThreshold;
		int 				threshold;

		int nFramesToAverage;
		int nFramesAveraged;
		bool 				bLearnBackground;
		
		ofxCv::ContourFinder contourFinder;

		void keyPressed(int key);
};
