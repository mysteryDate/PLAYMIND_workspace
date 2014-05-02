#include "Critter.h"

Critter::Critter(int numFrames) {

	hidden = false;
	currentFrame = ofRandom(numFrames);
	velocity = ofRandom(0, 10);
	direction = ofRandom(0, 2*PI);
	position = ofPoint(ofRandom(0,1920), ofRandom(0,1080));
	this-> numFrames = numFrames;

}

void Critter::update(vector< ofRectangle > hands) {

	ofVec2f nearestHand; // The vector to the closest hand
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

	if( direction > 2*PI)
		direction -= 2*PI;

	if(hands.size() != 0) {
		nearestHand = findClosestHand(hands); 
		float fear = 1/nearestHand.length() * 100;
		ofVec2f xhat(1,0);
		float angle = nearestHand.angle(xhat);
		float diff = direction - angle;
		if( (diff < PI and diff <= 0) or (diff < -PI and diff <= -2*PI) )
			direction += ofNoise( time ) * fear;
		if( diff != PI or diff != -PI)
			direction -= ofNoise( time ) * fear; // Turns sharper when it's closer
	}

}

// Returns a vector from critter to closest hand
ofVec2f Critter::findClosestHand(vector <ofRectangle> hands) {

	ofVec2f nearestHand;
	float minDist = 999999; // I hate doing this

	for (int i = 0; i < hands.size(); ++i)
	{
		ofPoint center = hands[i].getCenter();
		float dist = ofDistSquared(position.x, position.y, center.x, center.y);
		if(dist < minDist) {
			dist = minDist;
			nearestHand = ofVec2f(center.x - position.x, center.y - position.y);
		}
	}

	return nearestHand;
}