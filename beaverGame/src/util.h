#pragma once

#include "ofMain.h"
#include <cmath>

class util
{
public:

	vector< ofPolyline > transform(vector< ofPolyline > input, int dx, int dy, int z, int r = 0);
	ofPoint transform(ofPoint input, int dx, int dy, int z, int r = 0);
	vector< ofPoint > transform(vector< ofPoint > input, int dx, int dy, int z, int r = 0);

};