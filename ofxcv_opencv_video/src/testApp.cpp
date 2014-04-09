#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(320,200);
	#else
        vidPlayer.loadMovie("fingers.mov");
        vidPlayer.play();
	#endif

    colorImg.allocate(320,200);
	grayImage.allocate(320,200);
	grayBg.allocate(320,200);
	grayDiff.allocate(320,200);

	bLearnBakground = true;
	threshold = 80;
    
    int zoom = 1;
    
    firstPos = 0;
    secondPos = 0;
    
    tolerance = 0.00001;
    buffer = 10;

}

//--------------------------------------------------------------
void testApp::update(){

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), 320,200);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 320,200);
        #endif

        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, false);	// find holes
        /*
        edges.clear();
        
        if(contourFinder.nBlobs == 1) {
        	vector< ofPoint > points = contourFinder.blobs[0].pts;
            int maxX = 0;
            int maxY = 0;
            int minX = 320;
            int minY = 240;
            for (int i = 0; i < points.size(); i++) {
                if ( points[i].x > maxX)
                    maxX = points[i].x;
                if ( points[i].y > maxY)
                    maxY = points[i].y;
                if ( points[i].x < minX)
                    minX = points[i].x;
                if ( points[i].y < minY)
                    minY = points[i].y;
                if (points[i].y == 0 or points[i].y == 198 or points[i].x == 0 or points[i].x == 320) {
                    edges.push_back(points.at(i));
                    //cout << "edge!" << endl;
                }
            }
            //cout << "maxX: " << maxX << ", maxY: " << maxY << ", minX: " << minX << ", minY: " << minY << endl;
        }*/
    
        if(contourFinder.nBlobs == 1) {
            vector< ofPoint > &points = contourFinder.blobs[0].pts;
            int i = 0;
            bool firstEdgeFound = false;
            bool secondEdgeFound = false;
            while ( !firstEdgeFound and i < points.size()) {
                if(points[i].y == 0 or points[i].y == 198 or points[i].x == 0 or points[i].x == 320) {
                    firstEdgeFound = true;
                    firstEdge = points[i];
                    firstPos = i;
    //                cout << firstEdge.y << endl;
                }
                i++;
            }
            while( i < points.size()) {
                if(points[i].y == 0 or points[i].y == 198 or points[i].x == 0 or points[i].x == 320) {
                    secondEdgeFound = true;
                    secondEdge = points[i];
                    secondPos = i;
                    //cout << secondEdge.y << endl;
                }
                i++;
            }
            if(secondEdgeFound) {
                //wristOne = findWrist(firstPos, -1);
                //wristTwo = findWrist(secondPos, 1);
                //getAvgSlope(firstPos, -1, 20);
                //getAvgSlope(secondPos, 1, 20);
            }
            //findInflectionPoints();
            findKeyPoints();

        }
        
    }
    
    //cout << edges.size() << endl;
}

void testApp::findKeyPoints() {

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

void testApp::findInflectionPoints() {
    int h = 2;
    vector< ofPoint > &points = contourFinder.blobs[0].pts;
    int length = points.size();
    inflectionPoints.clear();
    float oldSlope;
    float oldSlopeChange;
    for (int i = 0; i < points.size() - 1; i++) {
        float newSlope;
        int nextIndex;
        int prevIndex;
        if( points[i + 1].x != points[i].x ) {
            newSlope = ( points[i + 1].y - points[i].y ) / ( points[i + 1].x - points[i].x );
            float newSlopeChange = newSlope - oldSlope;
            if( newSlopeChange * oldSlopeChange < 0) inflectionPoints.push_back(points[i]);
            oldSlope = newSlope;
            oldSlopeChange = newSlopeChange;
        }
    }
}

float testApp::getAvgSlope(int start, int d, int numPoints) {
    
    int i = start + d;
    float slope;
    int skipped = 0;
    vector< ofPoint > &points = contourFinder.blobs[0].pts;
    while( std::abs(i - start - skipped) ) {
        float newSlope;
        if(points[i].x != points[start].x) {
            newSlope = (points[i].y - points[start].y) / (points[i].x - points[start].x);
            slope += newSlope;
        }
        else skipped++;
        i += d;
    }
    slope /= (numPoints - skipped);
    return slope;
}


ofPoint testApp::findWrist(int start, int d){
    
    int i = start + d;
    float slope;
    float totalSlope = 0;
    bool wristFound = false;
    int skipped = 0;
    vector< ofPoint > &points = contourFinder.blobs[0].pts;
    while ( i > 0 and i < points.size() - 1 and !wristFound ) {
        float newSlope;
        if(points[i].x != points[start].x) {
            newSlope = (points[i].y - points[start].y) / (points[i].x - points[start].x);
            totalSlope += newSlope;
        }
        else skipped++;
        slope = totalSlope / std::abs(i - start - skipped);
        float PERCENTAGE_DIFF = std::abs( (slope - newSlope) / slope ) * (i - start - skipped);
        if( PERCENTAGE_DIFF > tolerance && std::abs(i - start - skipped) > buffer ) {
            wristFound = true;
            return points[i];
        }
        i += d;
    }
}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	//colorImg.draw(20,20);
	//grayImage.draw(360,20);
	//grayBg.draw(20,280);
	//grayDiff.draw(360,280);

	// then draw the contours:

	ofFill();
	ofSetHexColor(0x333333);
	ofRect(0,0,320,240);
	ofSetHexColor(0xffffff);
    
//    for (int i = 0; i < edges.size(); ++i){
//        ofCircle(edges[i], 3);
//    }
    
//    ofCircle(firstEdge, 3);
//    ofCircle(secondEdge, 3);


	// we could draw the whole contour finder
	//contourFinder.draw(0,0);

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(0,0);
    }
    
    ofSetColor(255,255,255);
//    ofLine(firstEdge, secondEdge);
    
//    ofCircle(wristOne, 4);
//    ofCircle(wristTwo, 4);
    
    if (contourFinder.nBlobs == 1) {
        vector< ofPoint > &points = contourFinder.blobs[0].pts;
//        int i = firstPos;
//        while( i > firstPos - 200 and i >= 0) {
//            ofCircle(points[i], 1);
//            //ofLine(firstEdge, points[i]);
//            i--;
//        }
//        i = secondPos;
//        while( i < secondPos + 200 and i < points.size()) {
//            ofCircle(points[i], 1);
//            i++;
//        }
        for (int i = 0; i < keyPoints.size(); i++) {
            ofCircle(keyPoints[i], 3);
        }
        ofSetColor(0,0,255);
        for (int i = 0; i < points.size(); i++) {
            ofCircle(points[i], 1);
        }
    }

    stringstream reportStream;
    reportStream << "t: " << tolerance << endl
    << "b: " << buffer << endl;
    ofDrawBitmapString(reportStream.str(), 200, 180);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
            if(vidPlayer.isPaused()) {
                vidPlayer.setPaused(false);
            }
            else vidPlayer.setPaused(true);
			break;
		case OF_KEY_RIGHT:
			if(vidPlayer.isPaused()) {
                vidPlayer.nextFrame();
            }
			break;
		case OF_KEY_LEFT:
			if(vidPlayer.isPaused()) {
                vidPlayer.previousFrame();
            }
            break;
        case '=':
            tolerance += 0.000001;
            break;
        case '-':
            tolerance -= 0.000001;
            break;
        case 'b':
            buffer++;
            break;
        case 'v':
            buffer--;
            break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
