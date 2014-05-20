// Calibration and regions
#pragma once

#ifdef __APPLE__
	#define PLATFORM "MAC"
#else
	#define PLATFORM "WINDOWS"
#endif

#include "ofMain.h"
// Computer vision
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
// Saving data
#include "ofxXmlSettings.h"
// My headers
#include "utility.h"
// Standards
#include <cmath>

#define REGISTRATION false

#define START_PHASE 7

class ofApp : public ofBaseApp{

public:
	void setup();
	void loadSettings();

	void update();
	void adjustPhase();
	void updateRegions();

	void draw();
	void drawFeedback();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);

		bool bCalibrateVideo;
		bool bCalibrateKinect;
		bool bDrawRegions;

		ofVideoPlayer firstVideo;
		ofVideoPlayer secondVideo;
		ofVideoPlayer* video;

		// Input processing
		ofxKinect 			kinect;	
		ofxCvGrayscaleImage	depthImage;
		ofxCvGrayscaleImage depthBackground;
		int 				nearThreshold;
		int 				farThreshold;
		bool 				bLearnBackground;
		// For cropping out un-desired regions
		cv::Mat 			input;
		cv::Mat 			croppedInput;

		int x, y, w, h;
		float z, r;

		map<string, ofPolyline> regions;

		ofxCv::ContourFinder ContourFinder;

		// For saving data
		ofxXmlSettings 		XML;
		// For determining what's going on
		int currentPhase;
		int nextPhaseFrame;

		// Calibration settings
		float video_x, video_y, video_w, video_h, video_r;
		float kinect_x, kinect_y, kinect_z;
		int crop_left, crop_top, crop_right, crop_bottom;
		// Feedback
		bool bDisplayFeedback;

		// Adding regions
		int 			activeRegion;
		vector<string> regionNames;

};
