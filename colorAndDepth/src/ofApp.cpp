#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	ofBackground(0,0,0);

	//kinect instructions
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();

	colorImage.allocate(kinect.width, kinect.height);
	colorImageBackground.allocate(kinect.width, kinect.height);
	depthImage.allocate(kinect.width, kinect.height);
	depthImageBackground.allocate(kinect.width, kinect.height);
	processedImage.allocate(kinect.width, kinect.height);
	processedImageBackground.allocate(kinect.width, kinect.height);
	bLearnBackground = false;

	ContourFinder.setMinArea(MIN_CONTOUR_AREA);

	cThreshold = 25;
	dThreshold = 176;

	bDrawFeedback = false;
	bShowColor = false;

	x = INPUT_DATA_DX;
	y = INPUT_DATA_DY;
	w = INPUT_DATA_ZOOM;

	background.loadImage("background.jpg");

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();

	if(kinect.isFrameNew()) {
		colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
		depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		if(bLearnBackground) {
			colorImageBackground = colorImage;
			depthImageBackground = depthImage;
			bLearnBackground = false;
		}

		// depthImage -= depthImageBackground;
		depthImage.threshold(dThreshold);  // Threshold out all but the nearest pixels

		processedImage = colorImage;
		processedImageBackground = colorImageBackground;
		processedImage.absDiff(processedImageBackground);
		processedImage.threshold(cThreshold);
        
		processedImage -= depthImage;

		ContourFinder.findContours(processedImage);
		contours = util::transform(ContourFinder, x, y, w);
	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	background.draw(0,0);
	ContourFinder.draw();
	ofPushMatrix();
	ofScale(w, w);
	ofTranslate(x, y);
	if(bShowColor)
		processedImage.draw(0,0);
	else
		depthImage.draw(0,0);
	ofPopMatrix();

	for (int i = 0; i < contours.size(); ++i)
	{
		contours[i].draw();
	}

	ofPushStyle();
	ofSetColor(0,0,0);
	ofRect(1420, 840, 300, 100);
	ofSetColor(0,255,0);
	stringstream reportStream;
	reportStream
	<< "cThreshold: " << cThreshold << endl
	<< "dThreshold: " << dThreshold << endl 
	// << "numCritters: " << Beavers.size() << endl
	<< "x: " << x << endl
	<< "y: " << y << endl
	<< "w: " << w << endl
	<< ofToString(ofGetFrameRate()) << endl
	<< ofToString(ofGetFrameNum()) << endl;

	ofDrawBitmapString(reportStream.str(), 1420, 840);
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch(key) {
		case 'b':
			bShowColor = !bShowColor;
			break;

		case ' ':
			bLearnBackground = true;
			break;

		case '>':
		case '.':
			cThreshold ++;
			if (cThreshold > 255) cThreshold = 255;
			break;
			
		case '<':
		case ',':
			cThreshold --;
			if (cThreshold < 0) cThreshold = 0;
			break;

		case '+':
		case '=':
			dThreshold ++;
			if (dThreshold > 255) dThreshold = 255;
			break;
			
		case '-':
			dThreshold --;
			if (dThreshold < 0) dThreshold = 0;
			break;

		case OF_KEY_LEFT: {
			x--;
			break;
		}

		case OF_KEY_RIGHT: {
			x++;
			break;
		}

		case OF_KEY_UP: {
			y--;
			break;
		}

		case OF_KEY_DOWN: {
			y++;
			break;
			}

		case 'W': {
			w+= 0.1;
			break;
		}	

		case 'w': {
			w-= 0.1;
			break;
		}	
	}

}


