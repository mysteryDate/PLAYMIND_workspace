#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
    
	ofSetFrameRate(60);

	img.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

	frame = 0;
	bSaveSequence = false;

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();

	if(kinect.isFrameNew()) {

		img.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE, false );

		if(bSaveSequence) {
			img.saveImage(ofToString(frame)+".jpg");
			frame++;
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	img.draw(0, 0, kinect.width, kinect.height);

	if(bSaveSequence) {
		ofPushStyle();
		ofSetColor(255,0,0);
		ofCircle(10, 10, 4);
		ofPopStyle();
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch(key) {
		case ' ':
			bSaveSequence = !bSaveSequence;
			break;
	}

}