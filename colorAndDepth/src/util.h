#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <cmath>

class util
{
public:

	static vector< ofPolyline > transform(vector< ofPolyline > input, int dx, int dy, float z, int r = 0);
	static ofPoint 				transform(ofPoint input, int dx, int dy, float z, int r = 0);
	static vector< ofPoint > 	transform(vector< ofPoint > input, int dx, int dy, float z, int r = 0);
	static vector< ofPolyline > transform(ofxCv::ContourFinder input, int dx, int dy, float z, int r = 0);

};