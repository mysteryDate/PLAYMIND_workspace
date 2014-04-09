#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class armBlob : public ofxCvBlob {

public:

	void simplify(float tolerance);

	// Constructor
	armBlob(ofxCvBlob blob, float tolerance);

	ofPoint wrist[2];
	vector< ofPoint > ends;

	ofPolyline simplifiedBlob;

	bool wristFound;
	bool endsFound;

	// How far within the border we'll look for ends
	int padding;

	// Finds points that leave the frame
	void findEnds(int minx, int miny, int maxx, int maxy);


private:

	void findWrist();
};