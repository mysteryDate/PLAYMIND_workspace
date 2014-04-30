#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0,0,0);
	ofSetFrameRate(60);

	//kinect instructions
	kinect.init();
	kinect.open();
	kinectImg.allocate(kinect.width, kinect.height);
	kinectBackground.allocate(kinect.width, kinect.height);
	kinectDiff.allocate(kinect.width, kinect.height);
	kinectBackground.set(0);
	bLearnBackground = false;
	threshold = 2;

	nFramesToAverage = 1;
	nFramesAveraged = 0;

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();

	if(kinect.isFrameNew()) {

		kinectImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		if(bLearnBackground == true) {
			learnBackground();
			nFramesAveraged++;
			if(nFramesAveraged == nFramesToAverage) {
				bLearnBackground = false;
			}
		}
		else{
			kinectDiff.absDiff(kinectImg, kinectBackground);
			kinectDiff.threshold(threshold);
			// contourFinder.findContours(kinectDiff);
		}
	}
 
}

void ofApp::learnBackground(){

	if(kinect.isFrameNew()) {

		unsigned char * pix = kinectImg.getPixels();
		int numPix = kinect.width * kinect.height;

		// Scale image
		for (int i = 0; i < numPix; ++i)
		{
			pix[i] /= nFramesToAverage;
		}

		kinectBackground += kinectImg;

	}

}


//--------------------------------------------------------------
void ofApp::draw(){
    
	kinectImg.draw(0,0);
	kinectBackground.draw(kinect.width + 10, 0);
	kinectDiff.draw(0, kinect.height + 10);
    ofPushMatrix();
    ofTranslate(0, kinect.height + 10);
    // contourFinder.draw();
    ofPopMatrix();

	ofPushStyle();
	ofSetColor(0,255,0);
	ofDrawBitmapString("threshold: "+ofToString(threshold), 100, 100);
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch(key) {

		case ' ':
			kinectBackground.set(0);
			bLearnBackground = true;
			nFramesAveraged = 0;
			break;

		case '>':
		case '.':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
			
		case '<':
		case ',':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;

	}

}
