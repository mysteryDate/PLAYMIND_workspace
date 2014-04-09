#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.init();
	kinect.open();

	grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	colorImg.allocate(kinect.width, kinect.height);
    
    ofBackground(50, 50, 50);

	width = kinect.width;
	height = kinect.height;

	nearThreshold = 230;
	farThreshold = 180;

	dx = 0;
	dy = 0;
	zoom = 1;

	ofSetFrameRate(120);
    
    bThreshWithOpenCV = false;
    modeSelection = true;

    findingMethod = 1;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();

	if(kinect.isFrameNew() and modeSelection) {

		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {

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
		contourFinder.findContours(grayImage, 20, (kinect.width*kinect.height)/2, 10, false);

		if(contourFinder.nBlobs == 1) {
			switch(findingMethod) {
				case 1:
					findGreatestDistances();
					break;
				case 2:
					findInflections();
					break;
			}
		}
	}

}

void ofApp::findGreatestDistances() {

	vector< ofPoint > &points = contourFinder.blobs[0].pts;
	int length = points.size();
	keyPoints.clear();

	float dminus2 = 0; // The distance two samples ago
	float dminus1 = 0; // The distance last sample

	for(int i = 1; i < length - 1; ++i) {
	    float m;
	    float b;
	    float dsquared;
	    m = (points[i + 1].y - points[i - 1].y) / (points[i + 1].x - points[i - 1].x);
	    b = points[i + 1].y - m * points[i + 1].x;
	    dsquared = (points[i].y - m * points[i].x - b) * (points[i].y - m * points[i].x - b) / (m * m + 1);

	    if(dminus1 - dminus2 > 0 and dsquared - dminus1 <= 0) { // The distance got bigger last sample, and is getting smaller now
	        keyPoints.push_back(points[i - 1]);
	    }

	    dminus2 = dminus1;  // Move my pointers (thanks Gary!)
	    dminus1 = dsquared;
	}
}

void ofApp::findInflections() {

	vector< ofPoint > &points = contourFinder.blobs[0].pts;
	int length = points.size();
	keyPoints.clear();
	float oldSlope;
	float oldSlopeChange;
	for (int i = 0; i < points.size() - 1; i++) {
	    float newSlope;
	    int nextIndex;
	    int prevIndex;
	    if( points[i + 1].x != points[i].x ) {
	        newSlope = ( points[i + 1].y - points[i].y ) / ( points[i + 1].x - points[i].x );
	        float newSlopeChange = newSlope - oldSlope;
	        if( newSlopeChange * oldSlopeChange < 0) keyPoints.push_back(points[i]);
	        oldSlope = newSlope;
	        oldSlopeChange = newSlopeChange;
	    }
	}

}

void ofApp::calibrateImage(ofxCvGrayscaleImage& image) {
    
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);

	if(modeSelection) {
		//grayImage.draw(dx, dy, zoom*width, zoom*height);
		contourFinder.draw(dx, dy, zoom*width, zoom*height);
		for( int i = 0; i < keyPoints.size(); ++i ) {
			ofCircle(keyPoints[i], 3);
		}
	}
	else {
		kinect.drawDepth(dx, dy, zoom*width, zoom*height);
	}

	ofSetColor(0, 255, 0);
	stringstream reportStream;

	reportStream << "dx: " << dx << endl 
	<< "dy: " << dy << endl
	<< "zoom: " << zoom << endl
    << "near threshold: " << nearThreshold << endl
    << "far threshold: " << farThreshold << endl
    << "thresh with open cv? " << bThreshWithOpenCV << endl
    << "framerate: " << ofToString(ofGetFrameRate()) << endl;

    switch(findingMethod) {
    	case 1:
    		reportStream << "Using greatest distance method" << endl;
    		break;
    	case 2:
    		reportStream << "Using inflection point method" << endl;
    		break;
    }

	ofDrawBitmapString(reportStream.str(), 1600, 900);
    
    ofSetColor(0, 255, 0);
    ofLine(dx + zoom*width/2, dy + zoom*height/4, dx + zoom*width/2, dy + 0.75*zoom*height);
    ofLine(dx + zoom*width/4, dy + zoom*height/2, dx + 0.75*zoom*width, dy + zoom*height/2);
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

		case 'z':
			zoom *= 1.01;
			break;

		case 'x':
			zoom *= 0.99;
			break;

		case 'p':
			ofLogNotice() << "dx: " << dx << ", dy: " << dy << ", zoom: " << zoom;
			ofLogNotice() << "nearThreshold: " << nearThreshold << ", farThreshold: " << farThreshold;
			break;
            
        case ' ':
            bThreshWithOpenCV = !bThreshWithOpenCV;
            break;

        case 'm':
        	modeSelection = !modeSelection;
        	break;

        case '1':
        	findingMethod = 1;
        	break;

        case '2':
        	findingMethod = 2;
        	break;

        case '3':
        	findingMethod = 3;
        	break;

	}
}

