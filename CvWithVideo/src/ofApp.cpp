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

	// tolerance = 9;

    contourFinder.setMinAreaRadius(20);

    int xMin = DISPLAY_WIDTH;
	int yMin = DISPLAY_HEIGHT;
	int xMax = 0;
	int yMax = 0;

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

		for (int i = 0; i < contourFinder.size(); ++i)
		{
			ofPolyline shape = contourFinder.getPolyline(i);
			for (int j = 0; j < shape.size(); ++j)
			{
				if(shape[j].x < xMin) xMin = shape[j].x;
				if(shape[j].x > xMax) xMax = shape[j].x;
				if(shape[j].y < yMin) yMin = shape[j].y;
				if(shape[j].y > yMax) yMax = shape[j].y;
			}
		}

		// cout << xMin << endl << yMin << endl << xMax << endl << yMax << endl;

		contourFinder.findEnds();

	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	//movie.draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	//grayImg.draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	contourFinder.draw();
	RectTracker& tracker = contourFinder.getTracker();

	drawLabels();
	drawEnds();
	
	ofPushStyle();
	ofSetColor(0, 255, 0);

	stringstream reportStream;
	reportStream;
	// << "Near threshold: " << nearThreshold << endl
	// << "Far threshold: " << farThreshold << endl
	// << "Tolerance: " << tolerance << endl
	// << "xMax: " << xMax << endl
	// << "yMax: " << yMax << endl
	// << "xMin: " << xMin << endl
	// << "yMin: " << yMin << endl;
	if(bPlaying)	reportStream << "Framerate: " << ofGetFrameRate() << endl;

	ofNoFill();
	vector< int > bounds = contourFinder.getBounds();
	ofRect(bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);

	ofDrawBitmapString(reportStream.str(), DISPLAY_WIDTH - 200, DISPLAY_HEIGHT - 50);
	ofPopStyle();

}

void ofApp::drawEnds() {

	ofPushStyle();

	for (int i = 0; i < contourFinder.size(); ++i)
	{
		if(contourFinder.ends[i].size() == 2) {
			ofCircle(contourFinder.ends[i][0], 3);
			ofCircle(contourFinder.ends[i][1], 3);
		}
	}

	ofPopStyle();

}

void ofApp::drawLabels() {

	ofPushStyle();
	ofSetColor(255,255,255);
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
	ofPopStyle();

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

		// case 't':
		// 	tolerance += 0.1;
		// 	break;

		// case 'r':
		// 	tolerance -= 0.1;
		// 	break;

		case OF_KEY_RIGHT:
			if(!bPlaying) movie.nextFrame();
            break;
            
        case OF_KEY_LEFT:
            if(!bPlaying) movie.previousFrame();
            break;

	}

}