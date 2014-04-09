#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class armBlob : public ofxCvBlob {

public:

	void simplify(float tolerance);

	// Constructor
	armBlob();

	ofPoint wrist[2];
	ofPoint end[2];

	ofPolyline simplifiedBlob;
};