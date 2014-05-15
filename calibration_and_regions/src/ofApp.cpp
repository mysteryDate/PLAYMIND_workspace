// Calibration and regions
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);

	firstVideo.loadMovie("videos/Map_Argenteuil_P1_v11.mov");
	firstVideo.setLoopState(OF_LOOP_NONE);
	secondVideo.loadMovie("videos/Map_Argenteuil_P2_v11.mov");
	secondVideo.setLoopState(OF_LOOP_NONE);
	// firstVideo.play();
	video = &firstVideo;
	video->setFrame(4272);
	currentPhase = 3;
	nextPhaseFrame = 5600;
	// nextPhaseFrame = video->getCurrentFrame() + 1;

	ofBackground(0,0,0);  
	XML.loadFile("settings.xml");
	loadSettings();

	kinect.init();
	kinect.setRegistration(REGISTRATION);
	kinect.open();
	depthImage.allocate(kinect.width, kinect.height);
	depthBackground.allocate(kinect.width, kinect.height);

	bCalibrateVideo = true;
	bCalibrateKinect = false;
	bDrawRegions = false;
	activeRegion = 0;

	// Creating regions
	// activeRegion = 0;
	// ofBuffer buffer = ofBufferFromFile("natives.txt");
	// while(!buffer.isLastLine() ) {
	// 	string name = buffer.getNextLine().c_str();
	// 	regions.insert(pair<string, ofPolyline>(name, ofPolyline()));
	// }
	activeRegion = 0;
	regions.insert(pair<string, ofPolyline>("La bataille\ndu long Sault", ofPolyline()));

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();
	adjustPhase();

	if(kinect.isFrameNew()) {

		depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		if(bLearnBackground) {
			depthBackground = depthImage;
			bLearnBackground = false;
		}
		// Background subtraction
		depthImage -= depthBackground;
		// Remove out of bounds
		unsigned char *pix = depthImage.getPixels();
		for (int i = 0; i < depthImage.getHeight() * depthImage.getWidth(); ++i)
		{
			if(pix[i] > nearThreshold || pix[i] < farThreshold)
				pix[i] = 0;
			else
				pix[i] = 255;
		}

		input = ofxCv::toCv(depthImage);
		cv::Rect crop_roi = cv::Rect(crop_left, crop_top, 
			kinect.width - crop_left - crop_right,
			kinect.height - crop_top - crop_bottom);
		croppedInput = input(crop_roi).clone();

		ContourFinder.findContours(croppedInput);
	}
 
}

//--------------------------------------------------------------
void ofApp::draw(){

	if(bCalibrateVideo){
		ofPushMatrix();
			video->draw(video_x, video_y, video_w, video_h);
		ofPopMatrix();
	}
	else
		video->draw(0,0);

	if(bDisplayFeedback)
		drawFeedback();

}

void ofApp::updateRegions() {

	regions.clear();
	XML.pushTag("PHASE", currentPhase);
	if(XML.getNumTags("REGIONS") > 0) {
		XML.pushTag("REGIONS");
		for (int i = 0; i < XML.getNumTags("REGION"); ++i)
		{
			XML.pushTag("REGION", i);
			string name = XML.getValue("NAME", "unknown");
			regions.insert(pair<string, ofPolyline>(name, ofPolyline()));
			for (int j = 0; j < XML.getNumTags("PT"); ++j)
			{
				int x = XML.getValue("PT:X", 0, j);
				int y = XML.getValue("PT:Y", 0, j);
				regions[name].addVertex(ofPoint(x,y));
			}
			XML.popTag();
		}
	    XML.popTag();
	}
    XML.popTag();

}

void ofApp::adjustPhase() {

	video->update();
	XML.pushTag("PHASEINFORMATION");

	int frame = video->getCurrentFrame();
	if( frame >= nextPhaseFrame) { // Change phase
		currentPhase++;
		if(currentPhase >= 9)
			currentPhase = 0;
		nextPhaseFrame = XML.getValue("PHASE:STARTFRAME", nextPhaseFrame + 1000, currentPhase + 1);
		if(currentPhase == 5) {
			secondVideo.setFrame(1900);
			secondVideo.update();
		}
		if(currentPhase == 6) {
			secondVideo.play();
			video = &secondVideo;
			firstVideo.stop();
			firstVideo.setFrame(0);
		}
		if(currentPhase == 0) {
			secondVideo.stop();
			secondVideo.setFrame(0);
			firstVideo.play();
			video = &firstVideo;
		}
		farThreshold = XML.getValue("PHASE:FARTHRESHOLD", 2, currentPhase);
		nearThreshold = XML.getValue("PHASE:NEARTHRESHOLD", 42, currentPhase);
		updateRegions();
	}
	XML.popTag();

	// // Phase 5 is the magical phase
	// if(currentPhase == 5) {
	// 	// ofImage img;
	// 	// img.setFromPixels(secondVideo.getPixels(), secondVideo.getWidth(), secondVideo.getHeight(), OF_IMAGE_COLOR);
	// 	// img.saveImage("second_video_still.png");
	// 	// secondVideo.update();
	// }

}

void ofApp::drawFeedback() {

	ofPushStyle();
	depthImage.draw(0,0);
	ofSetColor(0,255,0);
	ofTranslate(crop_left, crop_top);
		ContourFinder.draw();
	ofTranslate(-crop_left, -crop_top);

	auto iterator = regions.begin();
	int j = 0;
	while(j < activeRegion) {
		iterator++;
		j++;
	}

	ofPushStyle();
	ofPushMatrix();
	if(bCalibrateVideo) {
		ofTranslate(video_x, video_y);
		ofScale(video_w/video->getWidth(), video_h/video->getHeight());
	}
	int i = 0;
	for(auto iter=regions.begin(); iter!=regions.end(); ++iter) {
		ofSetColor( (i&1) * 255, (i&2) * 128, (i&4) * 64); // A color varying tool I'm quite proud of
		iter->second.draw();
		i++;
	}
	ofSetColor(255,255,255);
	iterator->second.draw();
	ofPopStyle();
	ofPopMatrix();

	stringstream reportStream;
	reportStream
	<< "frame: " << video->getCurrentFrame() << endl
	<< "currentPhase: " << currentPhase << endl
	<< "nextPhaseFrame: " << nextPhaseFrame << endl
	<< "framerate: " << ofToString(ofGetFrameRate()) << endl;
	if(bCalibrateVideo) {
		reportStream 
		<< "video_x: " << video_x << endl
		<< "video_y: " << video_y << endl
		<< "video_w: " << video_w << endl
		<< "video_h: " << video_h << endl;
	}
	if(bCalibrateKinect) {
		reportStream
		<< "kinect_x: " << kinect_x << endl
		<< "kinect_y: " << kinect_y << endl
		<< "kinect_z: " << kinect_z << endl;
	}
	if(bDrawRegions and regions.size() >= activeRegion) {
		if(regions.size() >= j) {
			string name = iterator->first;
			reportStream << name << endl;
		}
	}
	reportStream 
	<< "regions.size(): " << regions.size() << endl
	<< "activeRegion: " << activeRegion << endl;

	ofDrawBitmapString(reportStream.str(), 20, 600);
	ofPopStyle();

}

void ofApp::loadSettings() {

	XML.pushTag(ofToString(PLATFORM));
		video_x = XML.getValue("VIDEO:X", 0);
		video_y = XML.getValue("VIDEO:Y", 0);
		video_w = XML.getValue("VIDEO:W", firstVideo.getWidth());
		video_h = XML.getValue("VIDEO:H", firstVideo.getHeight());
		video_r = XML.getValue("VIDEO:R", 0);

		XML.pushTag("KINECT");
				XML.pushTag("CROP");
					crop_left = XML.getValue("LEFT", 0);
					crop_right = XML.getValue("RIGHT", 0);
					crop_top = XML.getValue("TOP", 0);
					crop_bottom = XML.getValue("BOTTOM", 0);
				XML.popTag();
			if(REGISTRATION) 
				XML.pushTag("REGISTRATION");
			else
				XML.pushTag("NOREGISTRATION");
					kinect_z = XML.getValue("Z", 2.77);
					kinect_x = XML.getValue("X", 0) + crop_left * kinect_z;
					kinect_y = XML.getValue("Y", 0) + crop_top * kinect_z;
				XML.popTag();
		XML.popTag();
	XML.popTag();

	// ContourFinder
	ContourFinder.setMinArea(XML.getValue("CV:MINAREA", 1000));

	farThreshold = XML.getValue("KINECT:FARTHRESHOLD", 2);
	nearThreshold = XML.getValue("KINECT:NEARTHRESHOLD", farThreshold + 40);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

switch(key) {

		case 'c': {
			if(bCalibrateVideo) {
				bCalibrateVideo = false;
				bCalibrateKinect = true;
			}
			else if(bCalibrateKinect) {
				bCalibrateKinect = false;
				bDrawRegions = true;
			}
			else if(bDrawRegions) {
				bDrawRegions = false;
				bCalibrateVideo = true;
			}
			break;
		}

		case OF_KEY_LEFT:
			if(bCalibrateVideo)
				video_x--; 
			if(bCalibrateKinect)
				kinect_x--;
			if(bDrawRegions) {
				activeRegion--;
				if(activeRegion < 0)
					activeRegion = regions.size() - 1;
			}
			break;

		case OF_KEY_RIGHT:
			if(bCalibrateVideo)
				video_x++; 
			if(bCalibrateKinect)
				kinect_x++;
			if(bDrawRegions) {
				activeRegion++;
				if(activeRegion >= regions.size())
					activeRegion = 0;
			}
			break;

		case OF_KEY_UP:
			if(bCalibrateVideo)
				video_y--; 
			if(bCalibrateKinect)
				kinect_y--;
			break;

		case OF_KEY_DOWN:
			if(bCalibrateVideo)
				video_y++; 
			if(bCalibrateKinect)
				kinect_y++;
			break;

		case 'Z':
			if(bCalibrateVideo) {
				video_w++; 
				video_h *= video_w/video->getWidth();
			}
			if(bCalibrateKinect)
				kinect_z+=0.01;
			break;

		case 'z':
			if(bCalibrateVideo) {
				video_w++; 
				video_h *= video_w/video->getWidth();
			}
			if(bCalibrateKinect)
				kinect_z-=0.01;
			break;

		case 'W': 
			video_w++;
			break;

		case 'w': 
			video_w--;
			break;

		case 'H': 
			video_h++;
			break;

		case 'h': 
			video_h--;
			break;

		case 'R': 
			video_r+= 0.1;
			break;

		case 'r': 
			video_r-= 0.1;
			break;

		case 'f':
			bDisplayFeedback = !bDisplayFeedback;
			break;

		case ' ': {
			if(video->isPaused())
				video->setPaused(false);
			else
				video->setPaused(true);
			break;
		}

		case OF_KEY_RETURN:
			bLearnBackground = true;
			break;

		case 'S': {
			// For saving regions
			XML.pushTag("PHASEINFORMATION");
				XML.pushTag("PHASE", currentPhase); // push phase, have to push in one at a time (annoying)
					if (XML.getNumTags("REGIONS") == 0)
						XML.addTag("REGIONS");
					XML.pushTag("REGIONS");
						XML.clear();
						for(auto iterator=regions.begin(); iterator!=regions.end(); ++iterator) {
							int rNum = XML.addTag("REGION");
							XML.setValue("REGION:NAME", iterator->first, rNum);
							XML.pushTag("REGION", rNum);
							for (int i = 0; i < iterator->second.size(); ++i)
							{
								int vNum = XML.addTag("PT");
								XML.setValue("PT:X", iterator->second[i].x, vNum);
								XML.setValue("PT:Y", iterator->second[i].y, vNum);
							}
							XML.popTag();
						}
					XML.popTag();
				XML.popTag();
			XML.popTag();
			// Saving calibration
			XML.pushTag(ofToString(PLATFORM));
				XML.pushTag("VIDEO");
					XML.setValue("X", video_x);
					XML.setValue("Y", video_y);
					XML.setValue("W", video_w);
					XML.setValue("H", video_h);
				XML.popTag();
				XML.pushTag("KINECT");
				if(REGISTRATION)
					XML.pushTag("REGISTRATION");
				else
					XML.pushTag("NOREGISTRATION");

						XML.setValue("X", kinect_x);
						XML.setValue("Y", kinect_y);
						XML.setValue("Z", kinect_z);
					XML.popTag();
				XML.popTag();
			XML.popTag();
			XML.saveFile("settings.xml");
			cout << "Settings saved!";
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	if(bDrawRegions) {
		auto iter = regions.begin();
		int j = 0;
		while(j < activeRegion) {
			iter++;
			j++;
		}
		string name = iter->first;
		regions[name].addVertex(x, y);
	}

}

