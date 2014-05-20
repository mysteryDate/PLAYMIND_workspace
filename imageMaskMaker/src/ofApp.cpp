#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

//	img.loadImage("riviereMask_v3.png");
	video.loadMovie("RiviereMask_03_.mov");
    video.setFrame(600);
    video.setPaused(true);

    img.allocate(video.getWidth(), video.getHeight(), OF_IMAGE_COLOR);
    img2.allocate(video.getWidth(), video.getHeight(), OF_IMAGE_COLOR_ALPHA);

//	unsigned char *pix = img.getPixels();

//	int numPix = img.getWidth() * img.getHeight() * 4;
//
//	for (int i = 0; i < numPix; i+=4)
//	{
//		unsigned char r = pix[i];
//		unsigned char g = pix[i + 1];
//		unsigned char b = pix[i + 2];
//		unsigned char a = pix[i + 3];
//
//		// Anything but black
//		if( !(r < 100 and g < 100 and b < 100) ) {
//			pix[i + 3] = 0;
//		}	
//		else {
//			pix[i] = 0;
//			pix[i + 1] = 0;
//			pix[i + 2] = 0;
//		}
//
//
//	}
    
//    img2.setFromPixels(img.getPixels(), img.getWidth(), img.getHeight(), OF_IMAGE_COLOR_ALPHA);
    
//    img2.saveImage("river_mask_V3_processed.png");

}

//--------------------------------------------------------------
void ofApp::update(){

    int frame = video.getCurrentFrame() + 1;
    video.setFrame(frame);
    video.update();
    video.setPaused(true);
    img.setFromPixels(video.getPixels(),video.getWidth(),video.getHeight(), OF_IMAGE_COLOR );
     
	unsigned char *pix = img.getPixels();
    unsigned char *outpix = img2.getPixels();

	int numPix = img.getWidth() * img.getHeight();

	int in, out;
	unsigned char r, g, b;

	for (int i = 0; i < numPix; i++)
	{
        in = i * 3;
        out = i * 4;
		r = pix[in];
		g = pix[in + 1];
		b = pix[in + 2];
        
        outpix[out] = r;
		outpix[out + 1] = g;
		outpix[out + 2] = b;
        unsigned char a = 255 - (r+g+b)/3;
        a = ofMap(a, 20, 255, 0, 255, true);
		outpix[out + 3] = a;

//		// Anything but black
//		if( !(r < 50 and g < 50 and b < 50) ) {
//			outpix[out] = 255;
//			outpix[out + 1] = 255;
//			outpix[out + 2] = 255;
//			outpix[out + 3] = 0;
//		}
//		else {
//			outpix[out] = 0;
//			outpix[out + 1] = 0;
//			outpix[out + 2] = 0;
//			outpix[out + 3] = 255;
//		}


	}

//   img2.setFromPixels(img.getPixels(), img.getWidth(), img.getHeight(), OF_IMAGE_COLOR_ALPHA);
    if(frame < 1000)
        img2.saveImage("animated_mask/0"+ofToString(frame)+".png");
    else
        img2.saveImage("animated_mask/"+ofToString(frame)+".png");
}

//--------------------------------------------------------------
void ofApp::draw(){

//	img2.draw(0,0);
    video.draw(0,0);
    ofPushStyle();
    ofSetColor(0, 255, 0);
    ofDrawBitmapString(ofToString(video.getCurrentFrame()), ofPoint(500,500));
    ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
