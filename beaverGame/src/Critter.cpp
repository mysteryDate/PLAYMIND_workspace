#include "Critter.h"

Critter::Critter(int numFrames) {

	hidden = false;
	currentFrame = ofRandom(numFrames);
	velocity = ofRandom(0, 10);
	direction = ofRandom(0, 360);
	position = ofPoint(ofRandom(0,1920), ofRandom(0,1080));

	timeOffset = ofMap(position.x + position.y, 0, 3000, -100, 100);

	this-> numFrames = numFrames;

	vectors[0] = ofVec2f(velocity * cos(direction/180*PI) * 10, velocity * sin(direction/180*PI) * 10);

}

void Critter::update(vector< ofRectangle > hands) {

	ofVec2f nearestHand; // The vector to the closest hand
	currentFrame++;
	if(currentFrame >= numFrames)
		currentFrame = 0;

	if(!hidden) {
		position.x += velocity * cos(direction/180*PI);
		position.y += velocity * -1*sin(direction/180*PI); // Because y descends upwards
	}
	else {
		velocity = 0;
	}

	float time = ofGetElapsedTimef();
	velocity += 	ofSignedNoise(time + timeOffset ) * VELOCITY_DISPLACEMENT_SCALE;
	direction += 	ofSignedNoise(time + timeOffset ) * DIRECTION_DISPLACEMENT_SCALE;

	if( direction > 360 )
		direction -= 360;
	if( direction < 0)
		direction += 360;

	vectors[0] = ofVec2f(velocity * cos(direction/180*PI) * 10, velocity * sin(direction/180*PI) * 10);

	if(hands.size() != 0) {
		nearestHand = findClosestHand(hands); 
		float fear = 1/nearestHand.length() * 10000;
		ofVec2f xhat(1,0);
        int dir = -1;
        if (velocity < 0) {
            dir = 1;
        }
		float angle = nearestHand.angle(xhat);
		float diff = direction - angle;
		if(diff < 0)
			diff += 360;
		if( diff < 170 )
            direction +=  fear; // Turns sharper when it's closer
		if( diff > 190 )
			direction -=  fear; // Turns sharper when it's closer
	}

	vectors[1] = ofVec2f(velocity * cos(direction/180*PI) * 10, velocity * sin(direction/180*PI) * 10);
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
			minDist = dist;
			nearestHand = ofVec2f(center.x - position.x, center.y - position.y);
		}
	}

	return nearestHand;
}