#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	kinect.setRegistration(true);
	kinect.init(false, false);
	kinect.open();

	grayImage.allocate(kinect.width, kinect.height);

	nearThreshold = 230;
	farThreshold = 70;

	ofSetFrameRate(60);

}

//--------------------------------------------------------------
void testApp::update(){

	kinect.update();
	if(kinect.isFrameNew() ) {
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

		grayImage.flagImageChanged();
		contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	grayImage.draw(0, 0, kinect.width, kinect.height);

	ofSetColor(255, 255, 255);
	stringstream reportStream;

	reportStream << "Far Threshold: " << farThreshold << endl
	<< "Near Threshold: " << nearThreshold << endl;
}

//--------------------------------------------------------------
void testApp::exit(){
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
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
