#pragma once

// Calibration variables
// -----------------------------------
// Cropping out kinect data
#define KINECT_CROP_LEFT 	28
#define KINECT_CROP_RIGHT 	10
#define KINECT_CROP_TOP 	14
#define KINECT_CROP_BOTTOM 	27

// Transforming kinect data to fit "real" world
#define INPUT_DATA_ZOOM 2.57
#define INPUT_DATA_DX	-25
#define	INPUT_DATA_DY	-100
#define INPUT_DATA_R 	0

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
		void updateBeavers();

		void draw();
		void drawBeavers();
		void drawHandOverlay();
		void drawFeedback();

		struct Beaver
		{
			Beaver() : hidden(false), currentFrame(0) {}
			float v;
			float d; //Direction, in degrees for now
			ofPoint p;
			int currentFrame;
			bool 	hidden;
			void draw(vector< ofImage > gif) {
				gif[currentFrame].draw(p.x, p.y);
			}
		};
		vector< Beaver > beavers;
		vector< ofImage > beaverFrames;

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
		
		// Cv
		ofxCv::ContourFinder contourFinder;

		// Background
		ofImage background;

		// Feedback
		bool bDrawFeedback;
		// Calibration
		float x, y, w, h, r;

		void keyPressed(int key);
};
