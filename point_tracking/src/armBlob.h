#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class armBlob : public ofxBlob {

public:

	void simplify(float tolerance);

	// Constructor
	armBlob();

	ofxPoint wrist[2];
	ofxPoint end[2];
};