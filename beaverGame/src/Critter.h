#pragma once

#include "ofMain.h"
#include <cmath>

// #define PI 3.14159265;
#define TIME_SCALE 5.0; // Smoothness of noisy motion
#define VELOCITY_DISPLACEMENT_SCALE 1; // How much velocity can change in a frame
#define DIRECTION_DISPLACEMENT_SCALE 0.087266; // 5 degrees

class Critter
{
public:
	Critter(int numFrames);

	float 	velocity;
	float 	direction; //Direction, in radians, like a boss
	ofPoint position;
	int 	currentFrame;
	bool 	hidden;
	int 	numFrames;

	void update();

};