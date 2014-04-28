#pragma once

// Calibration variables
// -----------------------------------
// Cropping out kinect data
#define KINECT_CROP_LEFT 28
#define KINECT_CROP_RIGHT 10
#define KINECT_CROP_TOP 14
#define KINECT_CROP_BOTTOM 27

// Transforming kinect data to fit "real" world
#define INPUT_DATA_ZOOM 2.57
#define INPUT_DATA_DX 40
#define	INPUT_DATA_DY -64
#define INPUT_DATA_R 0

// Transforming video data to fit maquette
#define VIDEO_X -69
#define VIDEO_Y -88
#define VIDEO_W 1866
#define VIDEO_H 1046
#define VIDEO_R 1.4
// -----------------------------------

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxRipples.h"
#include "ofxBounce.h" 
#include "ofxTrueTypeFontUC.h"
#include <cmath>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);

		ofxKinect 		kinect;
		ofVideoPlayer 	video;
		// ofVideoGrabber	videoGrabber;
		// cv::Mat 		input;
		ofImage 		image;

		ofxCvColorImage input;

		ofFile file;

		ofxCvColorImage background;
		ofxCvColorImage	diff;

		ofxCvGrayscaleImage grayImage;
		ofxCvGrayscaleImage red;
		ofxCvGrayscaleImage green;
		ofxCvGrayscaleImage blue;


		int threshold;
		bool bSaveSequence;

		
};
