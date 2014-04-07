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
        vector< ofPoint > points = blob.pts;
        entries[i] = points.at(vpoint);
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
        ofCircle(entries[i], 5);
	}
    
    stringstream reportStream;
    reportStream << "vpoint is: " << vpoint << endl;
    if(contourFinder.nBlobs >= 1) {
        reportStream << "number of points is: " << contourFinder.blobs.at(0).nPts << endl;
    }
    
    ofDrawBitmapString(reportStream.str(), 400, 400);
    
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
            
        case 'n':
            vpoint++;
            break;
            
        case 'b':
            vpoint++;
            break;
            

	}
}
