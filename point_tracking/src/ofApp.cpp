#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.init();
	kinect.open();

	grayImage.allocate(kinect.width, kinect.height);
    
    ofBackground(50, 50, 50);

	width = kinect.width;
	height = kinect.height;

	nearThreshold = 230;
	farThreshold = 180;

	dx = 0;
	dy = 0;
	zoom = 1;

	ofSetFrameRate(60);

    modeSelection = true;
    
    simplifyTolerance = 10;

    paused = false;

}

//--------------------------------------------------------------
void ofApp::update(){

	if(!paused) kinect.update();

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

		contourFinder.findContours(grayImage, 20, (kinect.width*kinect.height)/2, 10, false);

		if(contourFinder.nBlobs != 0) {
			arms.clear();
			for (int i = 0; i < contourFinder.nBlobs; ++i)
			{
				armBlob arm = armBlob(contourFinder.blobs[i], simplifyTolerance);
				arm.findEnds(0, 0, width, height);
				arms.push_back(arm);
			}
		}
	}

}

void ofApp::simplify(ofxCvBlob &blob){
	
    ofPolyline line = ofPolyline(blob.pts);
    line.simplify(simplifyTolerance);
    polyLines.push_back(line);
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);

	if(!modeSelection) {
		//grayImage.draw(dx, dy, zoom*width, zoom*height);
		kinect.drawDepth(dx, dy, zoom*width, zoom*height);
	}
	contourFinder.draw(dx, dy, zoom*width, zoom*height);
	for (int i = 0; i < arms.size(); ++i)
	{
		arms[i].simplifiedBlob.draw();
		for (int j = 0; j < arms[i].ends.size(); ++j)
		{
			ofCircle(arms[i].ends[j], 3);
		}
	}

	ofSetColor(0, 255, 0);
	stringstream reportStream;

	reportStream << "dx: " << dx << endl 
	<< "dy: " << dy << endl
	<< "zoom: " << zoom << endl
    << "near threshold: " << nearThreshold << endl
    << "far threshold: " << farThreshold << endl
    << "framerate: " << ofToString(ofGetFrameRate()) << endl
    << "simplifyTolerance: " << simplifyTolerance << endl;

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

        case 'm':
        	modeSelection = !modeSelection;
        	break;
            
        case 't':
        	simplifyTolerance += 0.1;
            break;

        case 'r':
        	simplifyTolerance -= 0.1;
        	break;

        case ' ':
        	paused = !paused;
        	break;


	}
}

