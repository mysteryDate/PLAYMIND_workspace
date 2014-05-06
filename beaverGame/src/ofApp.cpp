 #include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){


	// ofSetDataPathRoot("../Resources/data/");

	ofSetFrameRate(60);
	background.loadImage("background.jpg");

	//kinect instructions
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
	colorImage.allocate(kinect.width, kinect.height);
	colorImageBackground.allocate(kinect.width, kinect.height);
	depthImage.allocate(kinect.width, kinect.height);
	processedImage.allocate(kinect.width, kinect.height);
	processedImageBackground.allocate(kinect.width, kinect.height);
	bLearnBackground = false;

	ContourFinder.setMinArea(MIN_CONTOUR_AREA);

	cThreshold = 25;
	dThreshold = 170;

	bDrawFeedback = false;

	// x = INPUT_DATA_DX;
	// y = INPUT_DATA_DY;
	// w = INPUT_DATA_ZOOM;

	// TODO magic num
	for (int i = 0; i < NUM_FRAMES; ++i)
	{
		ofImage img;
		img.loadImage("beaver/beaver-"+ofToString(i)+".png");
		img.resize(img.getWidth()*0.2, img.getHeight()*0.2);
		img.setAnchorPercent(0.5,0.5);
		gifFrames.push_back(img);
	}

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();

	if(kinect.isFrameNew()) {

		colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
		depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		if(bLearnBackground) {
			colorImageBackground = colorImage;
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
		Hands = util::transform(ContourFinder, INPUT_DATA_DX, INPUT_DATA_DY, INPUT_DATA_ZOOM);
		for (int i = 0; i < Hands.size(); ++i)
		{
			Hands[i] = Hands[i].getSmoothed(4);
		}
	}

	updateBeavers();
 
}

//--------------------------------------------------------------
void ofApp::updateBeavers() {


	float Iw = gifFrames[0].getWidth();
	float Ih = gifFrames[0].getHeight();

	if(ofGetFrameNum() % 30 == 0 && Beavers.size() < 120) {
		Critter newBeaver = Critter(NUM_FRAMES); 
		Beavers.push_back(newBeaver);
	}

	for (int i = 0; i < Beavers.size(); ++i)
	{
		Beavers[i].update(Hands);
		Critter * B = &Beavers[i];
		vector< ofPoint > corners;
		for (int j = 0; j < 4; ++j)
		{
			int xd = (j & 1)*2 - 1;
			int yd = (j & 2) - 1;
			float x = B->p.x + cos(B->d*PI/180)*Iw*xd/2 + sin(B->d*PI/180)*Ih*yd/2;
			float y = B->p.y - sin(B->d*PI/180)*Iw*xd/2 + cos(B->d*PI/180)*Ih*yd/2;
			corners.push_back(ofPoint(x,y));
		}
		// Beaver has left the building
		if(B->p.x > ofGetWindowWidth() + Iw or B->p.x < 0 - Iw or B->p.y > ofGetWindowHeight() + Iw or B->p.y < 0 - Iw)
				Beavers.erase(Beavers.begin() + i);
		// Collision
		B->hidden = false; 
		for (int j = 0; j < Hands.size(); ++j)
		{
			if( Hands[j].inside(B->p.x, B->p.y) )  {
				B->v = 0;
				B->hidden = true;
				continue;
			}
			for (int k = 0; k < corners.size(); ++k)
			{
				if( Hands[j].inside(corners[k]) ) {
					B->v = 0;
					B->hidden = true;
				}
			}
		}

	}

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    background.draw(0,0);
	drawBeavers();
    // drawHandOverlay();
    if(bDrawFeedback)
	    drawFeedback();

}

void ofApp::drawBeavers() {

	for (int i = 0; i < Beavers.size(); ++i)
	{		
		Critter CB = Beavers[i];
		if(!CB.hidden) {
			ofPushMatrix();
	        ofTranslate(CB.p.x, CB.p.y); // Translate to the center of the beaver
			ofRotateZ(CB.d);
				gifFrames[CB.currentFrame].setAnchorPercent(0.5,0.5); // So that we draw from the middle
				gifFrames[CB.currentFrame].draw(0,0);
			ofPopMatrix();
		}
	}
}

void ofApp::drawHandOverlay() {

	ofPushStyle();
	ofPushMatrix();
		ofFill();
		ofTranslate(INPUT_DATA_DX, INPUT_DATA_DY);
		ofScale(INPUT_DATA_ZOOM, INPUT_DATA_ZOOM);

	for (int i = 0; i < ContourFinder.size(); ++i)
	{
		ofSetColor(0,0,0);
		ofPolyline blob = ContourFinder.getPolyline(i);
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
	processedImage.draw(0,0);
	ContourFinder.draw();
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

	ofDrawBitmapString(reportStream.str(), 1420, 880);

	for (int i = 0; i < Hands.size(); ++i)
	{
		Hands[i].draw();
	}

	for (int i = 0; i < Beavers.size(); ++i)
	{

		map< int, ofVec2f > vectors = Beavers[i].vectors;
		ofPushStyle();
		for (int j = 0; j < vectors.size(); ++j)
		{
			ofSetColor(0, 255, 0);
			ofLine(Beavers[i].p.x, Beavers[i].p.y, Beavers[i].p.x + vectors[j].x, Beavers[i].p.y + vectors[j].y);
		}
		ofPopStyle();
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

		case 'f':
			bDrawFeedback = !bDrawFeedback;
			break;

		case 'b': {
			Critter newBeaver = Critter(NUM_FRAMES); // Arg is number of frames
			Beavers.push_back(newBeaver);
			break;
		}

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
