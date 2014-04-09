#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofPoint findWrist(int start, int direction);
        float getAvgSlope(int start, int direction, int numPoints);

        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif

        ofxCvColorImage			colorImg;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBakground;
    
        //Points at the edges
        vector< ofPoint > edges;
    
        ofPoint firstEdge;
        ofPoint secondEdge;
        int firstPos;
        int secondPos;

        ofPoint wristOne;
        ofPoint wristTwo;
    
        float tolerance;
        int buffer;
    
        float avgSlopeOne;
        float avgSlopeTwo;
    
        void findInflectionPoints();
        vector< ofPoint > inflectionPoints;

};

