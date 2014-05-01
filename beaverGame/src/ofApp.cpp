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
	bLearnBackground = false;

	contourFinder.setMinArea(MIN_CONTOUR_AREA);

	nearThreshold = 15;
	farThreshold = 1;

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();

	if(kinect.isFrameNew()) {

		kinectImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

		if(bLearnBackground == true) {
			kinectBackground = kinectImg;
			bLearnBackground = false;
		}

		kinectImg -= kinectBackground;

		// Threshold by hand, because we need two planes 
		// And I don't want to throw out the other information (by saturating)
		unsigned char *pix = kinectImg.getPixels();
		for (int i = 0; i < kinectImg.width * kinectImg.height; ++i)
		{
			if(pix[i] > farThreshold and pix[i] < nearThreshold)
				pix[i] = 255;
			else
				pix[i] = 0;
		}
		kinectImg.flagImageChanged();

		// Now I crop using ROIs
		input = ofxCv::toCv(kinectImg);
		cv::Rect crop_roi = cv::Rect(KINECT_CROP_LEFT, KINECT_CROP_TOP, 
			kinect.width - KINECT_CROP_LEFT - KINECT_CROP_RIGHT,
			kinect.height - KINECT_CROP_TOP - KINECT_CROP_BOTTOM);
		croppedInput = input(crop_roi).clone();

		contourFinder.findContours(kinectImg);
	}
 
}

//--------------------------------------------------------------
void ofApp::draw(){
    
	kinectImg.draw(0,0);
	contourFinder.draw();

    drawFeedback();
}

void ofApp::drawFeedback(){

	ofPushStyle();
	ofSetColor(0,255,0);

	stringstream reportStream;
	reportStream
	<< "nearThreshold: " << nearThreshold << endl
	<< "farThreshold: " << farThreshold << endl
	<< ofToString(ofGetFrameRate()) << endl
	<< ofToString(ofGetFrameNum()) << endl;

	ofDrawBitmapString(reportStream.str(), 1420, 880);
	ofPopStyle();


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch(key) {

		case ' ':
			bLearnBackground = true;
			break;

		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;

		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;

	}

}
