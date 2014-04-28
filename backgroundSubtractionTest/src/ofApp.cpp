#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	kinect.init();
	kinect.open();

	video.loadMovie("Map_Argenteuil_v5.mov");
	video.play();

	ofBackground(0,0,0);

	// videoGrabber.initGrabber(kinect.width, kinect.height);

	image.allocate(kinect.width, kinect.height, OF_IMAGE_COLOR);
	grayImage.allocate(640, 480);
	red.allocate(640, 480);
	green.allocate(640, 480);
	blue.allocate(640, 480);

	background.allocate(640, 480);
	diff.allocate(640, 480);

	video.setFrame(1300);

	bSaveSequence = false;

	int threshold = 100;

}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();
	video.update();
	// if(video.getCurrentFrame() == 1300)
	// 	bSaveSequence = true;
	if(video.getCurrentFrame() < 1300)
		video.setFrame(1300);

	if(video.getCurrentFrame() >= 1300) {

		grayImage.set(0);
		
		bool fileExists = false;
		string name;
		int i = 0;
		while(!fileExists) {
			name = "backgrounds/"+ofToString(video.getCurrentFrame() - i)+".jpg";
			if(file.doesFileExist(name)) {
				fileExists = true;
			}
			i++;
		}
		cout << i << endl;

		image.loadImage(name);
		background.setFromPixels(image.getPixels(), 640, 480);

		diff.setFromPixels(kinect.getPixels(), 640, 480);

		unsigned char *dpix = diff.getPixels();
		unsigned char *bpix = background.getPixels();

		// diff -= background;
		// Manually for absolute values
		for (int i = 0; i < 640*480*3; i++)
		{
			int differance = abs(dpix[i] - bpix[i]);
			dpix[i] = differance;
		}

		diff.convertToGrayscalePlanarImages(red, green, blue);
		grayImage += red;
		grayImage += green;
		grayImage += blue;

		unsigned char *pix = grayImage.getPixels();

		int numPix = grayImage.getWidth() * grayImage.getHeight();

		for (int i = 0; i < numPix; i++)
		{
			if(pix[i] > threshold)
				pix[i] = 255;
			else
				pix[i] = 0;
		}

		diff.convertToRange(0, 255);
	}

	// if(kinect.isFrameNew()) {

	// 	image.setFromPixels(kinect.getPixels(), kinect.width, kinect.height, OF_IMAGE_COLOR);
	// 	background.loadImage("backgrounds/"+ofToString(video.getCurrentFrame())+".jpg");


	// 	if(bSaveSequence) {
	// 		image.saveImage("backgrounds/"+ofToString(video.getCurrentFrame())+".jpg");
	// 	}


	// }

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofRotateZ(VIDEO_R);
		video.draw(VIDEO_X, VIDEO_Y, VIDEO_W, VIDEO_H);
	ofRotateZ(-VIDEO_R);
	grayImage.draw(0,0);
	// diff.draw(0,0);

	ofPushStyle();
		ofSetColor(0,255,0);
		ofDrawBitmapString("threshold: "+ofToString(threshold), 10, 1000);
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
		case '=':
			threshold++;
			break;
		case '-':
			threshold--;
			break;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::exit() {

	kinect.close();
}
