#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.init();
	kinect.open();

	grayImage.allocate(kinect.width, kinect.height);

	int width = kinect.width;
	int height = kinect.height;

	nearThreshold = 230;
	farThreshold = 172;

	dx = 0;
	dy = 0;

	ofSetFrameRate(60);

	ofLogNotice() << "kinect.width: " << width << ", kinect.height: " << height;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();

	if(kinect.isFrameNew() ) {
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

		unsigned char * pix = grayImage.getPixels();
		
		int numPixels = grayImage.getWidth() * grayImage.getHeight();
		for(int i = 0; i < numPixels; i++) {
			if(pix[i] < nearThreshold && pix[i] > farThreshold) {
				pix[i] = 255;
			} else {
				pix[i] = 0;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);

	grayImage.draw(dx, dy, displayWidth, displayHeight);
	contourFinder.draw(dx, dy, displayWidth, displayHeight);
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
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

		case OF_KEY_UP:
			dy--;
			break;
			
		case OF_KEY_DOWN:
			dy++; 
			break;
			
		case OF_KEY_LEFT:
			dx--;
			break;
			
		case OF_KEY_RIGHT:
			dx++;
			break;

	}
}

