#pragma once

// Calibration variables
// -----------------------------------
// Transforming kinect data to fit "real" world
#define INPUT_DATA_ZOOM 2.67
#define INPUT_DATA_DX	-65
#define	INPUT_DATA_DY	-198
#define INPUT_DATA_R 	0

// For the contour finder
#define MIN_CONTOUR_AREA 1000

// For animation
#define NUM_FRAMES 24
// -----------------------------------

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "Critter.h"
#include "util.h"
#include <cmath>

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

		vector< Critter > Beavers;
		vector< ofImage > gifFrames;
		vector< ofPolyline > Hands;

		// Input Processing
		ofxKinect 				kinect;
		ofxCvColorImage 		colorImage;
		ofxCvColorImage 		colorImageBackground;
		ofxCvGrayscaleImage 	depthImage;
		ofxCvGrayscaleImage 	processedImage;
		ofxCvGrayscaleImage 	processedImageBackground;
		int 					cThreshold;
		int 					dThreshold;
		bool 					bLearnBackground;
		
		// Cv
		ofxCv::ContourFinder ContourFinder;

		// Background
		ofImage background;

		// Feedback
		bool bDrawFeedback;
		// Calibration
		float x, y, w, h, r;

		void keyPressed(int key);
};
