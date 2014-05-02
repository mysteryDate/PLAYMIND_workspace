 #include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	// ofSetDataPathRoot("../Resources/data/");

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

	// TODO magic num
	for (int i = 0; i < 12; ++i)
	{
		ofImage img;
		img.loadImage("beaver/"+ofToString(i)+".gif");
		img.resize(img.getWidth()*0.2, img.getHeight()*0.2);
		gifFrames.push_back(img);
	}

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

	updateBeavers();
 
}

//--------------------------------------------------------------
void ofApp::updateBeavers() {


	float imgWidth = gifFrames[0].getWidth();
	float imgHeight = gifFrames[0].getHeight();
	vector< ofRectangle > handRects;

	// Get the hand bounding rects in proper reference frame
	for (int i = 0; i < contourFinder.size(); ++i)
	{
		ofRectangle cbRect = ofxCv::toOf(contourFinder.getBoundingRect(i));
		float tx = cbRect.getMinX() * INPUT_DATA_ZOOM + INPUT_DATA_DX;
		float ty = cbRect.getMinY() * INPUT_DATA_ZOOM + INPUT_DATA_DY;
		float tw = cbRect.getWidth() * INPUT_DATA_ZOOM;
		float th = cbRect.getHeight() * INPUT_DATA_ZOOM;
		ofRectangle transRect = ofRectangle(tx, ty, tw, th);
		handRects.push_back(transRect);
	}

	if(ofGetFrameNum() % 30 == 0 && Beavers.size() < 1) {
		// New beaver time, TODO magic numbers
		Critter newBeaver = Critter(12); // Arg is number of frames
		Beavers.push_back(newBeaver);
	}

	for (int i = 0; i < Beavers.size(); ++i)
	{
		Beavers[i].update(handRects);
		ofRectangle bRect = ofRectangle(Beavers[i].position.x, Beavers[i].position.y, imgWidth, imgHeight);
		// Beaver has left the building
		if(bRect.getMinX() > 1920 or bRect.getMaxX() < 0 or bRect.getMinY() > 1080 or bRect.getMaxY() < 0)
				Beavers.erase(Beavers.begin() + i);
		// Collision
		Beavers[i].hidden = false; 
		for (int j = 0; j < handRects.size(); ++j)
		{
			if( handRects[j].intersects(bRect) )  {
				Beavers[i].hidden = true;
			}
		}

	}

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // background.draw(0,0);
	drawBeavers();
    // drawHandOverlay();
    if(bDrawFeedback)
	    drawFeedback();

}

void ofApp::drawBeavers() {

	for (int i = 0; i < Beavers.size(); ++i)
	{		
		Critter CB = Beavers[i];
		if(!CB.hidden)
			gifFrames[CB.currentFrame].draw(CB.position.x, CB.position.y);
	}
}

void ofApp::drawHandOverlay() {

	ofPushStyle();
	ofPushMatrix();
		ofFill();
		ofTranslate(INPUT_DATA_DX, INPUT_DATA_DY);
		ofScale(INPUT_DATA_ZOOM, INPUT_DATA_ZOOM);

	for (int i = 0; i < contourFinder.size(); ++i)
	{
		ofSetColor(0,0,0);
		ofPolyline blob = contourFinder.getPolyline(i);
		blob = blob.getSmoothed(4);
		ofPoint center = blob.getCentroid2D();

		ofBeginShape();
			for (int j = 0; j < blob.size(); ++j) {
				ofVertex(blob[j]);
			}
		ofEndShape();

		ofSetColor(255,255,255);
		// ofCircle(center, 3);

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
	<< "nearThreshold: " << nearThreshold << endl
	<< "farThreshold: " << farThreshold << endl 
	<< "numCritters: " << Beavers.size() << endl
	// << "x: " << x << endl
	// << "y: " << y << endl
	<< ofToString(ofGetFrameRate()) << endl
	<< ofToString(ofGetFrameNum()) << endl;

	ofDrawBitmapString(reportStream.str(), 1420, 880);

	vector< ofRectangle > handRects;

	// Get the hand bounding rects in proper reference frame
	for (int i = 0; i < contourFinder.size(); ++i)
	{
		ofRectangle cbRect = ofxCv::toOf(contourFinder.getBoundingRect(i));
		float tx = cbRect.getMinX() * INPUT_DATA_ZOOM + INPUT_DATA_DX;
		float ty = cbRect.getMinY() * INPUT_DATA_ZOOM + INPUT_DATA_DY;
		float tw = cbRect.getWidth() * INPUT_DATA_ZOOM;
		float th = cbRect.getHeight() * INPUT_DATA_ZOOM;
		ofRectangle transRect = ofRectangle(tx, ty, tw, th);
		handRects.push_back(transRect);
	}

	// Where it thinks the Beavers are
	float imgWidth = gifFrames[0].getWidth();
	float imgHeight = gifFrames[0].getHeight();
	for (int i = 0; i < Beavers.size(); ++i)
	{
		// for (int j = 0; j < contourFinder.size(); ++j)
		// {
		// 	ofRectangle cbRect = ofxCv::toOf(contourFinder.getBoundingRect(j));
		// 	float tx = cbRect.getMinX() * INPUT_DATA_ZOOM + INPUT_DATA_DX;
		// 	float ty = cbRect.getMinY() * INPUT_DATA_ZOOM + INPUT_DATA_DY;
		// 	float tw = cbRect.getWidth() * INPUT_DATA_ZOOM;
		// 	float th = cbRect.getHeight() * INPUT_DATA_ZOOM;
			// ofRect(tx, ty, tw, th);
			// ofPolyline line = contourFinder.getPolyline(i);
			// ofPolyline hand;
			// for (int k = 0; k < line.size(); ++k)
			// {
			// 	hand.addVertex(line[k].x * INPUT_DATA_ZOOM + INPUT_DATA_DX, line[k].y * INPUT_DATA_ZOOM + INPUT_DATA_DY);
			// }
			// hand.draw();
		// }
		map< int, ofVec2f > vectors = Beavers[i].vectors;
		ofPushStyle();
		for (int j = 0; j < vectors.size(); ++j)
		{
			ofSetColor(0, 255*j, 0);
			ofLine(Beavers[i].position.x, Beavers[i].position.y, Beavers[i].position.x + vectors[j].x, Beavers[i].position.y + vectors[j].y);
		}
		ofPopStyle();
		// ofRectangle bRect = ofRectangle(Beavers[i].position.x, Beavers[i].position.y, imgWidth, imgHeight);
		// Collision
		// int cx = (bRect.getCenter().x - INPUT_DATA_DX) / INPUT_DATA_ZOOM;
		// int cy = (bRect.getCenter().y - INPUT_DATA_DY) / INPUT_DATA_ZOOM;
		// ofPoint center = ofPoint(cx, cy);
		// ofCircle(center, 3);
		// ofNoFill();
		// ofRect(bRect);
		// ofCircle(ofPoint(bRect.getCenter().x, bRect.getCenter().y), 3);

	}

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

		case 'b':
			Critter newBeaver = Critter(12); // Arg is number of frames
			Beavers.push_back(newBeaver);
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
