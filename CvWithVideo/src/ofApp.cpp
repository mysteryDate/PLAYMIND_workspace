#include "ofApp.h"

// There are dead spots on the video, this is the take
#define LEFT_PAD	20
#define RIGHT_PAD	35
#define TOP_PAD		70
#define BOTTOM_PAD	10


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

	hand_radius = 110;

	// tolerance = 9;

    contourFinder.setMinAreaRadius(20);

    // int xMin = DISPLAY_WIDTH;
	// int yMin = DISPLAY_HEIGHT;
	// int xMax = 0;
	// int yMax = 0;

	contourFinder.setBounds(LEFT_PAD, TOP_PAD, DISPLAY_WIDTH - RIGHT_PAD, DISPLAY_HEIGHT - BOTTOM_PAD);

}

//--------------------------------------------------------------
void ofApp::update(){

	movie.update();

	if(movie.isFrameNew() ) {

		flow.calcOpticalFlow(movie);

		colorImg.setFromPixels(movie.getPixels(), DISPLAY_WIDTH, DISPLAY_HEIGHT);
		grayImg = colorImg;
        // grayImg.setROI(LEFT_PAD, TOP_PAD, DISPLAY_WIDTH - LEFT_PAD - RIGHT_PAD, DISPLAY_HEIGHT - TOP_PAD - BOTTOM_PAD);

		unsigned char * pix = grayImg.getPixels();
		
		int numPixels = grayImg.getWidth() * grayImg.getHeight();
		for(int i = 0; i < numPixels; i++) {
		    if(pix[i] > nearThreshold or pix[i] < farThreshold) {
		        pix[i] = 0;
		    } 
		    // Trim the image because I don't know how ROIs work
		    if(i < TOP_PAD * DISPLAY_WIDTH || i > numPixels - (BOTTOM_PAD * DISPLAY_WIDTH) ||
		    	i % DISPLAY_WIDTH < LEFT_PAD || i % DISPLAY_WIDTH > DISPLAY_WIDTH - RIGHT_PAD) {
		    	pix[i] = 0;
		    }
		}

        // grayImg.setROI(LEFT_PAD, TOP_PAD,  DISPLAY_WIDTH - LEFT_PAD - RIGHT_PAD, DISPLAY_HEIGHT - TOP_PAD - BOTTOM_PAD);
		contourFinder.findContours(grayImg);
		contourFinder.update();
		//grayImg.resetROI();
    

		for (int i = 0; i < contourFinder.size(); ++i)
		{
			if(contourFinder.handFound[i])
				contourFinder.updateArm(i);
			else {
				contourFinder.findHand(i);
			}
			// ofPolyline shape = contourFinder.getPolyline(i);
			// for (int j = 0; j < shape.size(); ++j)
			// {
			// 	if(shape[j].x < xMin) xMin = shape[j].x;
			// 	if(shape[j].x > xMax) xMax = shape[j].x;
			// 	if(shape[j].y < yMin) yMin = shape[j].y;
			// 	if(shape[j].y > yMax) yMax = shape[j].y;
			// }
		}

		// cout << xMin << endl << yMin << endl << xMax << endl << yMax << endl;

		//contourFinder.findEnds();

	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofClear(0,0,0);
	//movie.draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	//grayImg.draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	//contourFinder.draw();
	

    
    ofPushStyle();
	ofSetColor(0, 255, 0);

    for (int i = 0; i < contourFinder.size(); i++) {
    	if(contourFinder.handFound[i]) {

    		
    		ofPolyline rt;
	        vector< ofPoint > line = contourFinder.getPolyline(i).getVertices();
		    ofSetColor(255, 255, 255);
	        ofCircle(contourFinder.tips[i], 3);
	        ofNoFill();
	        //ofCircle(contourFinder.tips[i], hand_radius);
	        //ofCircle(contourFinder.tips[i], hand_radius - 40);
	        ofFill();
	        ofSetColor(255,0,0); 
	        // ofCircle(contourFinder.wrists[i][0], 3);
	        // ofCircle(contourFinder.wrists[i][1], 3);
		    //ofCircle(contourFinder.getPolyline(i).getClosestPoint(ofPoint(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2)), 3);
	 	    // ofSetColor(0, 255, 255);
	 	    // ofPoint pt = toOf(contourFinder.getCentroid(i));
	 	    // ofCircle(pt, 3);
	 	    // ofSetColor(255, 255, 0);
	 	    // pt = toOf(contourFinder.getCenter(i));
	 	    // ofCircle(pt, 3);
	 	    // ofPoint cen = ofPoint((contourFinder.ends[i][0].x + contourFinder.ends[i][1].x)/2, (contourFinder.ends[i][0].y + contourFinder.ends[i][1].y)/2);
			// float m = (pt.y - cen.y) / (pt.x - cen.x);
			// float xn = (5000 - cen.y) / m + cen.x;
			// ofLine(cen.x, cen.y, xn, 5000);
	 	    // rt = toOf(contourFinder.getMinAreaRect(i));
	 	    // rt.draw();
	 	    // rt = toOf(contourFinder.getFitQuad(i));
	 	    // ofSetColor(255, 0, 255);
	 	    // rt.draw();
	 	    // rt = toOf(contourFinder.getFitEllipse(i));
	 	    // ofSetColor(0,0,255);
	 	    // rt.draw();  
	 	    // ofSetColor(255,50,255);
	 	    // rt = toOf(contourFinder.getConvexHull(i));
	 	    // rt.draw();
		    //contourFinder.hands[i].draw();
    	}
	    ofSetColor(0, 255, 0);

	    //features = flow.getFeatures();
	    for (int j = 0; j < features.size(); ++j)
	    {
	    	ofCircle(features[j], 3);
	    }
	    contourFinder.simplifiedPolylines[i].draw();
    	//ofPolyline hull = toOf(contourFinder.getConvexHull(i));
    	//hull.draw();
    }
	stringstream reportStream;

	vector<cv::Vec4i> defects;
	ofPoint centroid;
	ofPoint center;
	int simpPoints = 0;
    if(contourFinder.size() != 0) {
		defects = contourFinder.getConvexityDefects(0);
		simpPoints = contourFinder.simplifiedPolylines[0].size();
		ofPoint centroid = toOf(contourFinder.getCentroid(0));
		ofPoint center = toOf(contourFinder.getCenter(0));\
		reportStream << "Distance of centers: " << ofDist(centroid.x, centroid.y, center.x, center.y) << endl;
	}

	//flow.draw();


	if(bPlaying)	reportStream << "Framerate: " << ofGetFrameRate() << endl;
	reportStream
	// << "Near threshold: " << nearThreshold << endl
	// << "Far threshold: " << farThreshold << endl
	// << "Tolerance: " << tolerance << endl
	// << "xMax: " << xMax << endl
	// << "yMax: " << yMax << endl
	// << "xMin: " << xMin << endl
	// << "yMin: " << yMin << endl
	// << "#Defects: " << defects.size() << endl
	<< "Simplified points " << simpPoints << endl
	<< "Hand radius: " << hand_radius << endl
	<< "Frame: " << movie.getCurrentFrame() << endl;
	if(simpPoints > 9) {
		reportStream << "Hand is spread!" << endl;
	}

	ofNoFill();
	vector< int > bounds = contourFinder.getBounds();
	//ofRect(bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);

	ofSetColor(255, 255, 255);
	ofDrawBitmapString(reportStream.str(), DISPLAY_WIDTH - 200, DISPLAY_HEIGHT - 80);
	ofPopStyle();
    
    drawLabels();
	drawEnds();

	//grayImg.drawROI(0,0);
    //grayImg.resetROI();

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

        case 'T': {
        	//Features to track?
        	vector< ofVec2f > feat;
        	feat.resize(2);
        	feat[0].set(contourFinder.wrists[0][0].x, contourFinder.wrists[0][0].y);
        	feat[1].set(contourFinder.wrists[0][1].x, contourFinder.wrists[0][1].y);

        	flow.setFeaturesToTrack(feat);
        	break; }

        case 'H':
        	hand_radius++;
        	break;

        case 'h':
        	hand_radius--;
        	break;

	}

}