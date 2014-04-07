#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.init();
	kinect.open();

	grayImage.allocate(kinect.width, kinect.height);
	colorImg.allocate(kinect.width, kinect.height);
    
    ofBackground(50, 50, 50);

	width = kinect.width;
	height = kinect.height;

	nearThreshold = 230;
	farThreshold = 182;

	ofSetFrameRate(60);
    
    modeSelection = true;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();

	if(kinect.isFrameNew() and modeSelection) {

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
        
		contourFinder.findContours(grayImage, 20, (kinect.width*kinect.height)/2, 10, false);
	}

	for (int i = 0; i < contourFinder.nBlobs; ++i)
	{
		ofxCvBlob blob = contourFinder.blobs.at(i);
		centroids[i] = blob.centroid;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);

	if(modeSelection) {
		grayImage.draw(0, 0, width, height);
		contourFinder.draw(0, 0, width, height);
	}
	else {
		kinect.draw(0, 0, width, height);
	}

	ofSetColor(0,255,0);

	for (int i = 0; i < contourFinder.nBlobs; ++i)
	{
		ofCircle(centroids[i], 10);
	}

	// stringstream reportStream;

	// reportStream << "near threshold: " << nearThreshold << endl
 //    << "far threshold: " << farThreshold << endl
 //    << "thresh with open cv? " << bThreshWithOpenCV << endl
 //    << "framerate: " << ofToString(ofGetFrameRate()) << endl;

	// ofDrawBitmapString(reportStream.str(), 1600, 900);
    
    //ofSetColor(0, 255, 0);
    //ofLine(dx + zoom*width/2, dy + zoom*height/4, dx + zoom*width/2, dy + 0.75*zoom*height);
    //ofLine(dx + zoom*width/4, dy + zoom*height/2, dx + 0.75*zoom*width, dy + zoom*height/2);
    
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
            
        case 'm':
        	modeSelection = !modeSelection;
        	break;

	}
}

