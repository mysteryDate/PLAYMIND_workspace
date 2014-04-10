#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){

	movie.loadMovie("arms.mov");
	bPlaying = false;

	colorImg.allocate(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	grayImg.allocate(DISPLAY_WIDTH, DISPLAY_HEIGHT);

	nearThreshold = 250;
	farThreshold = 200;

	ofSetFrameRate(60);
	ofBackground(0,0,0);

	tolerance = 9;

    contourFinder.setMinAreaRadius(20);


}

//--------------------------------------------------------------
void ofApp::update(){

	movie.update();

	if(movie.isFrameNew() ) {

		colorImg.setFromPixels(movie.getPixels(), DISPLAY_WIDTH, DISPLAY_HEIGHT);
		grayImg = colorImg;

		unsigned char * pix = grayImg.getPixels();
		
		int numPixels = grayImg.getWidth() * grayImg.getHeight();
		for(int i = 0; i < numPixels; i++) {
		    if(pix[i] > nearThreshold or pix[i] < farThreshold) {
		        pix[i] = 0;
		    } 
		}

		contourFinder.findContours(grayImg);
		arms.clear();

		for (int i = 0; i < contourFinder.size(); ++i)
		{
			ofPolyline shape = contourFinder.getPolyline(i);
			shape.simplify(tolerance);
			armBlob arm;
			arm.label = contourFinder.getLabel(i);
			arm.shape = shape;
			arms.push_back(arm);
		}

	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	//movie.draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	//grayImg.draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	contourFinder.draw();
	RectTracker& tracker = contourFinder.getTracker();

	ofPushStyle();
	ofSetColor(0, 255, 0);
	for (int i = 0; i < arms.size(); ++i)
	{
		arms[i].shape.draw();
	}

	drawLabels();


	stringstream reportStream;
	reportStream << "Near threshold: " << nearThreshold << endl
	<< "Far threshold: " << farThreshold << endl
	<< "Tolerance: " << tolerance << endl
	<< "Framerate: " << ofGetFrameRate() << endl;
	ofDrawBitmapString(reportStream.str(), DISPLAY_WIDTH - 200, DISPLAY_HEIGHT - 50);
	ofPopStyle();

}

void ofApp::drawLabels() {

	RectTracker& tracker = contourFinder.getTracker();

	for (int i = 0; i < contourFinder.size(); ++i)
	{
		ofPoint center = toOf(contourFinder.getCenter(i));
		ofPushMatrix();
		ofTranslate(center.x, center.y);
		int label = contourFinder.getLabel(i);
		string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
		ofDrawBitmapString(msg, 0, 0);
		ofPopMatrix();
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch(key) {

		case ' ':
			if(!bPlaying) {
				movie.play();
				bPlaying = true;
			}
			else {
				movie.stop();
				bPlaying = false;
			}
			break;

		case '.':
			farThreshold++;
			break;

		case ',':
			farThreshold--;
			break;

		case '=':
			nearThreshold++;
			break;

		case '-':
			nearThreshold--;
			break;

		case 't':
			tolerance += 0.1;
			break;

		case 'r':
			tolerance -= 0.1;
			break;

		case OF_KEY_RIGHT:
			if(!bPlaying) movie.nextFrame();
            break;
            
        case OF_KEY_LEFT:
            if(!bPlaying) movie.previousFrame();
            break;

	}

}