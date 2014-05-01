#include "Critter.h"

Critter::Critter(int numFrames) {

	hidden = false;
	currentFrame = ofRandom(numFrames);
	velocity = ofRandom(0, 10);
	direction = ofRandom(0, 2*PI);
	position = ofPoint(ofRandom(0,1920), ofRandom(0,1080));
	this-> numFrames = numFrames;

}

void Critter::update() {

	currentFrame++;
	if(currentFrame >= numFrames)
		currentFrame = 0;

	if(!hidden) {
		position.x += velocity * sin(direction);
		position.y += velocity * cos(direction);
	}
	else {
		velocity = 0;
	}

	float time = ofGetElapsedTimef();
	velocity += 	ofSignedNoise( time ) * VELOCITY_DISPLACEMENT_SCALE;
	direction += 	ofSignedNoise( time ) * DIRECTION_DISPLACEMENT_SCALE;

}