#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	background.loadImage("background.jpg");

	//kinect instructions
	kinect.init();
	kinect.open();
	kinectImg.allocate(kinect.width, kinect.height);
	kinectBackground.allocate(kinect.width, kinect.height);
	bLearnBackground = false;

	contourFinder.setMinArea(MIN_CONTOUR_AREA);

	nearThreshold = 15;
	farThreshold = 1;

	bDrawFeedback = false;

	// x = INPUT_DATA_DX;
	// y = INPUT_DATA_DY;
	// w = INPUT_DATA_ZOOM;

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
    
    background.draw(0,0);

    drawHandOverlay();

    if(bDrawFeedback)
	    drawFeedback();
}

void ofApp::drawHandOverlay() {

	ofPushStyle();
	ofPushMatrix();
		ofSetColor(0,0,0);
		ofFill();
		ofTranslate(INPUT_DATA_DX, INPUT_DATA_DY);
		ofScale(INPUT_DATA_ZOOM, INPUT_DATA_ZOOM);

	for (int i = 0; i < contourFinder.size(); ++i)
	{
		ofPolyline blob = contourFinder.getPolyline(i);
		blob = blob.getSmoothed(4);
		ofPoint center = blob.getCentroid2D();

		ofBeginShape();
			for (int j = 0; j < blob.size(); ++j) {
				ofVertex(blob[j]);
			}
		ofEndShape();

		ofSetColor(255,255,255);
		ofCircle(center, 3);

	}

	ofPopStyle();
	ofPopMatrix();

}

void ofApp::drawFeedback(){

	ofPushStyle();
	kinectImg.draw(0,0);
	contourFinder.draw();
	ofSetColor(0,0,0);
	ofRect(1420, 840, 300, 100);
	ofSetColor(0,255,0);

	stringstream reportStream;
	reportStream
	// << "nearThreshold: " << nearThreshold << endl
	// << "farThreshold: " << farThreshold << endl
	// << "x: " << x << endl
	// << "y: " << y << endl
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

		case 'f':
			bDrawFeedback = !bDrawFeedback;
			break;

		// case OF_KEY_LEFT:
		// 	x--;
		// 	break;

		// case OF_KEY_RIGHT:
		// 	x++;
		// 	break;

		// case OF_KEY_UP:
		// 	y--;
		// 	break;

		// case OF_KEY_DOWN:
		// 	y++;
		// 	break;

		

	}

}
