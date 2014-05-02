#pragma once

#include "ofMain.h"
#include <cmath>

// #define PI 3.14159265;
#define TIME_SCALE 5.0; // Smoothness of noisy motion
#define VELOCITY_DISPLACEMENT_SCALE 1; // How much velocity can change in a frame
#define DIRECTION_DISPLACEMENT_SCALE 5; // 5 degrees

class Critter
{
public:
	Critter(int numFrames);

	float 	velocity;
	float 	direction; //Direction, in degrees, because what was I thinking?
	ofPoint position;
	int 	currentFrame;
	bool 	hidden;
	int 	numFrames;

	float 	timeOffset;

	map < int, ofVec2f > vectors;

	void update(vector< ofRectangle > hands);

private:

	ofVec2f findClosestHand(vector< ofRectangle > hands);

};