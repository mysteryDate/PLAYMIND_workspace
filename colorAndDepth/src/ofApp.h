#pragma once


// Transforming kinect data to fit "real" world
#define INPUT_DATA_ZOOM 2.57
#define INPUT_DATA_DX	-25
#define	INPUT_DATA_DY	-100
#define INPUT_DATA_R 	0

// Same, but for RGB camera
#define COLOR_INPUT_DATA_ZOOM 	2.77
#define COLOR_INPUT_DATA_DX		-91
#define	COLOR_INPUT_DATA_DY		-234
#define COLOR_INPUT_DATA_R 		0

// For the contour finder
#define MIN_CONTOUR_AREA 1000

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "util.h"
#include <cmath>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);

		ofxKinect 		kinect;

		ofxCvColorImage 		colorImage;
		ofxCvColorImage 		colorImageBackground;
		ofxCvGrayscaleImage 	depthImage;
		ofxCvGrayscaleImage 	depthImageBackground;

		ofxCvGrayscaleImage 	processedImage;
		ofxCvGrayscaleImage 	processedImageBackground;

		bool 					bLearnBackground;

		ofxCv::ContourFinder 	ContourFinder;

		int 					cThreshold;
		int 					dThreshold;

		vector< ofPolyline > 	contours;

		ofImage 				background;

		// Feedback
		bool bDrawFeedback;
		bool bShowColor;
		// Calibration
		float x, y, w, h, r;

		
};
