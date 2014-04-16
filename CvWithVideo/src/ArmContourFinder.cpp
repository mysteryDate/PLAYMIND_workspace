#include "ArmContourFinder.h"

ArmContourFinder::ArmContourFinder() {

	bounds.push_back(19);
	bounds.push_back(50);
	bounds.push_back(605);
	bounds.push_back(478);

	tolerance = 3;

	simplifiedPolylines.resize(polylines.size());

	MIN_HAND_SIZE = 80;
	MAX_HAND_SIZE = 150;
	SURVIVAL_FRAMES = 0;
	MAX_MOVEMENT_DISTANCE = 500;
	SMOOTHING_RATE = 1;

}

void ArmContourFinder::update() {

	//To run every frame
	int size = polylines.size();

	simplifiedPolylines.resize(size);
	ends.resize(size);
	wrists.resize(size);
	tips.resize(size);
	skippedFrames.resize(size);

	endIndeces.resize(size);
	wristIndeces.resize(size);
	tipIndeces.resize(size);

	handFound.resize(size, false);
	hands.resize(size);

	for (int i = 0; i < polylines.size(); ++i)
	{
		skippedFrames[i].resize(5, 0);
		simplifiedPolylines[i] = polylines[i].getSmoothed(SMOOTHING_RATE);
		// simplifiedPolylines[i].simplify(tolerance);
	}

}

void ArmContourFinder::findHand(int n) {

	ends[n].clear();
	ends[n].resize(2);
	if(polylines[n].size() > 10) {
		ends[n] = findEnds(n);
		ofPoint newTip = findTip(n);

		tips[n] = newTip;

		wrists[n] = findWrist(n, newTip);
	}

	// float d1 = ofDist(ends[n][0].x, ends[n][0].y, newTip.x, newTip.y);
	// float d2 = ofDist(ends[n][1].x, ends[n][1].y, newTip.x, newTip.y);
// 
	// if(d1 > MIN_HAND_SIZE or d2 > MIN_HAND_SIZE) {
	// 	tips[n] = newTip;
	// 	vector < ofPoint > newWrist = findWrist(n, newTip);
	// 	if(newWrist.size() == 2) {
	// 		wrists[n] = newWrist;
	// 		handFound[n] = true;
	// 	}
	// 	else
	// 		handFound[n] = false;
	// }
	// else {
	// 	handFound[n] = false;
	// }

}

void ArmContourFinder::updateArm(int n) {

	ofPoint * keypoints[] = {&ends[n][0], &ends[n][1], &tips[n], &wrists[n][0], &wrists[n][1]};

	ofVec2f velocity = ofxCv::toOf(getVelocity(n));
	float maxDist = MAX_MOVEMENT_DISTANCE - velocity.length();

	vector< ofPoint > newEnds = findEnds(n);
	ofPoint newTip = findTip(n);
	vector< ofPoint > newWrists = findWrist(n, newTip);
	if(newEnds.size() < 2 or newWrists.size() != 2) {
		for (int i = 0; i < skippedFrames[n].size(); ++i)
		{
			skippedFrames[n][i]++;
		}
		return;
	}
 	
	ofPoint newKeypoints [] = {newEnds[0], newEnds[1], newTip, newWrists[0], newWrists[1]};

	for (int i = 0; i < 5; ++i)
	{
		/* code */
		if(skippedFrames[n][i] > SURVIVAL_FRAMES) {
			*keypoints[i] = newKeypoints[i];
			skippedFrames[n][i] = 0;
			handFound[n] = false;
		}
		else{
			float dist = ofDist(newKeypoints[i].x, newKeypoints[i].y, keypoints[i]->x, keypoints[i]->y);
			if(dist <= maxDist) {
				float smoothedX = ofLerp(newKeypoints[i].x, keypoints[i]->x, SMOOTHING_RATE);
				float smoothedY = ofLerp(newKeypoints[i].y, keypoints[i]->y, SMOOTHING_RATE);
				newKeypoints[i] = ofPoint(smoothedX, smoothedY);
				*keypoints[i] = newKeypoints[i];
				skippedFrames[n][i] = 0;
			}
			else {
				skippedFrames[n][i]++;
			}
		}
	}

	//For detected arms
	// ofPoint * keypoints[] = {&ends[i][0], &ends[i][1], &tips[i], &wrists[i][0], &wrists[i][1]};

	// ofPoint nearest[5];
	// float distances[5];
	// int MAX_DISTANCE = 500;
	
	// for (int j = 0; j < 5; ++j)
	// {
	// 	unsigned int vertexIndex;
	// 	simplifiedPolylines[i].getClosestPoint(*keypoints[j], &vertexIndex);
	// 	nearest[j] = simplifiedPolylines[i][vertexIndex];
	// 	distances[j] = ofDistSquared(nearest[j].x, nearest[j].y, keypoints[j]->x, keypoints[j]->y);
	// 	if(distances[j] <= MAX_DISTANCE) {
	// 		*keypoints[j] = nearest[j];
	// 	}
	// 	else
	// 		handFound[i] = false;
	// }
}

vector< ofPoint > ArmContourFinder::findEnds(int n) {
	vector< ofPoint > pts = polylines[n].getVertices();
	vector< ofPoint > endPoints;

	for (int i = 0; i < pts.size(); ++i)
	{
		if(pts[i].x == bounds[0] || pts[i].y <= bounds[1] + 2
			|| pts[i].x >= bounds[2] - 2 || pts[i].y >=  bounds[3] - 2) {
			endPoints.push_back(pts[i]);
		}
	}
	if(endPoints.size() >= 2) {
		//Let's make it hte most distant
		float maxDist = 0;
		for (int i = 1; i < endPoints.size(); ++i)
		{
			float dist = ofDistSquared(endPoints[0].x, endPoints[0].y, endPoints[i].x, endPoints[i].y);
			if(dist > maxDist) {
				maxDist = dist;
				endPoints[1] = endPoints[i];
			}
		}
		endPoints.resize(2);
	}
	return endPoints;
}

void ArmContourFinder::setBounds(int  xMin, int yMin, int xMax, int yMax ) {
	bounds[0] = xMin;
	bounds[1] = yMin;
	bounds[2] = xMax;
	bounds[3] = yMax;
}

void ArmContourFinder::setTolerance(float tolerance) {
	this->tolerance = tolerance;
}

vector< int >& ArmContourFinder::getBounds() {
	return bounds;
}

// bool ArmContourFinder::findEnd(int i) {

// 	vector< ofPoint > pts = polylines[i].getVertices();
// 	vector< ofPoint > possibleEnds;
// 	vector< int > possibleEndIndeces;
// 	ends[i].clear();
// 	ends[i].resize(2);

// 	for (int i = 0; i < pts.size(); ++i)
// 	{
// 		if(pts[i].x == bounds[0] || pts[i].y <= bounds[1] + 2
// 			|| pts[i].x >= bounds[2] - 2 || pts[i].y >=  bounds[3] - 2) {
// 			possibleEnds.push_back(pts[i]);
// 		}
// 	}
// 	if(possibleEnds.size() >= 2) {
// 		ends[i][0] = possibleEnds[0];
// 		ends[i][1] = possibleEnds.back();
// 		polylines[i].getClosestPoint(ends[i][0], &endIndeces[i][0]);
// 		polylines[i].getClosestPoint(ends[i][1], &endIndeces[i][1]);
// 		return true;
// 	}
// 	else return false; 
// }

ofPoint ArmContourFinder::findTip(int n) {

	//Create a line connecting the center of the base of the arm to the center of the bounding box
	ofPoint boxCenter = ofxCv::toOf(getCenter(n));
	ofPoint baseCenter = ofPoint((ends[n][0].x + ends[n][1].x)/2, (ends[n][0].y + ends[n][1].y)/2 );
	// Slope of the line
	float m = (boxCenter.y - baseCenter.y) / (boxCenter.x - baseCenter.x);
	float yn = 5000;	// New y coordinate (far off)
	if(boxCenter.y < baseCenter.y) 
		yn *= -1;
	float xn = (yn - baseCenter.y) / m + baseCenter.x;
	ofPoint mostDistant = ofPoint(xn, yn);

	return simplifiedPolylines[n].getClosestPoint(mostDistant);

}

vector < ofPoint > ArmContourFinder::findWrist(int n, ofPoint newTip) {

	//Split points
	ofPolyline sideOne;
	ofPolyline sideTwo;

	unsigned int start;
	unsigned int end1;
	unsigned int end2;

	simplifiedPolylines[n].getClosestPoint(tips[n], &start);
	simplifiedPolylines[n].getClosestPoint(ends[n][0], &end1);
	simplifiedPolylines[n].getClosestPoint(ends[n][1], &end2);

	int minSquared = MIN_HAND_SIZE * MIN_HAND_SIZE;
	int maxSquared = MAX_HAND_SIZE * MAX_HAND_SIZE;

	// Put all to the left in one
	int i = start;
	float distSquared;
	while(i != end1 and i != end2) {
		distSquared = ofDistSquared(tips[n].x, tips[n].y, simplifiedPolylines[n][i].x, simplifiedPolylines[n][i].y );
		if(distSquared < maxSquared and distSquared > minSquared)
			sideOne.addVertex( simplifiedPolylines[n][i] );
		i++;
		if(i == simplifiedPolylines[n].size())
			i = 0;
	}
    i = start;
	while(i != end1 and i != end2) {
		distSquared = ofDistSquared(tips[n].x, tips[n].y, simplifiedPolylines[n][i].x, simplifiedPolylines[n][i].y );
		if(distSquared < maxSquared and distSquared > minSquared)
			sideTwo.addVertex( simplifiedPolylines[n][i] );
		i--;
		if(i < 0)
			i = simplifiedPolylines[n].size() - 1;
	}

	// Now find the two points with the shortest distance (how's about some n^2!)
	float shortestDist = 999999;
	vector< ofPoint > possibleWrists;
	possibleWrists.resize(2);
	for (int i = 0; i < sideOne.size(); ++i)
	{
		for (int j = 0; j < sideTwo.size(); ++j)
		{
			distSquared = ofDistSquared(sideOne[i].x, sideOne[i].y, sideTwo[j].x, sideTwo[j].y);
			if(distSquared < shortestDist) {
				possibleWrists[0] = sideOne[i];
				possibleWrists[1] = sideTwo[j];
				shortestDist = distSquared;
			}
		}
	}

	return possibleWrists;

	// float avgHandSize = (MAX_HAND_SIZE + MIN_HAND_SIZE) / 2;

	// vector< ofPoint > possibleWrists;
	// possibleWrists.resize(2);
	// vector< ofPoint > pts = simplifiedPolylines[n].getVertices();

	// float lowestDists[] = {50000, 50000}; 

	// unsigned int tipIndex;
	// simplifiedPolylines[n].getClosestPoint(newTip, &tipIndex);

	// unsigned int endIndex;
	// simplifiedPolylines[n].getClosestPoint(ends[n][0], &endIndex);



	// for (int i = 0; i < pts.size(); ++i)
	// {
	// 	float dist = ofDist(pts[i].x, pts[i].y, newTip.x, newTip.y);
	// 	if(dist < MAX_HAND_SIZE and dist > MIN_HAND_SIZE) {
	// 		if(i < tipIndex and dist - avgHandSize < lowestDists[0]) {
	// 			possibleWrists[0] = pts[i];
	// 			lowestDists[0] = dist;
	// 		}
	// 		else if(dist - avgHandSize < lowestDists[1]) {
	// 			possibleWrists[1] = pts[i];
	// 			lowestDists[1] = dist;
	// 		}
	// 	}
	// }

	// if( lowestDists[0] == 50000 or lowestDists[1] == 50000)
	// 	possibleWrists.clear();
	// return possibleWrists;

}

// bool ArmContourFinder::findWrist(int i) {

// 	int method = 2;
// 	wrists[i].resize(2);
// 	wristIndeces[i].resize(2);

// 	switch(method) {
// 		case 1: { //Pushback method
// 			int pushBack = 50; // Amount of verteces we go back for the wrist

// 			int a = tipIndeces[i] - pushBack;
// 			int b = tipIndeces[i] + pushBack;

// 			if(a < 0) {
// 				a += polylines[i].size();
// 			}
// 			if(b > polylines[i].size()) {
// 				b -= polylines[i].size();
// 			}

// 			wristIndeces[i][0] = a;
// 			wristIndeces[i][1] = b;
// 			wrists[i][0] = polylines[i].getVertices()[a];
// 			wrists[i][1] = polylines[i].getVertices()[b];
// 			break; 
// 		}

// 		case 2: {// Next simplified vertex method
// 			//simplifiedPolylines[i] = polylines[i];
// 			//simplifiedPolylines[i].simplify(tolerance);
// 			vector< ofPoint > pts = simplifiedPolylines[i].getVertices();

// 			unsigned int nearestEnds[2];
// 			simplifiedPolylines[i].getClosestPoint(ends[i][0], &nearestEnds[0]);
// 			simplifiedPolylines[i].getClosestPoint(ends[i][1], &nearestEnds[1]);

// 			unsigned int nearestTip;
// 			simplifiedPolylines[i].getClosestPoint(tips[i], &nearestTip);

// 			int a = nearestEnds[1] - nearestEnds[0];
// 			int b = pts.size() - a;

// 			if(a > b) {
// 				wrists[i][0] = pts[nearestEnds[0] + 1];
// 				wrists[i][1] = pts[nearestEnds[1] - 1];
// 				polylines[i].getClosestPoint(wrists[i][0], &wristIndeces[i][0]);
// 				polylines[i].getClosestPoint(wrists[i][1], &wristIndeces[i][1]);
// 			}
// 			else {
// 				wrists[i][0] = pts[nearestEnds[0] - 1];
// 				wrists[i][1] = pts[nearestEnds[1] + 1];
// 				polylines[i].getClosestPoint(wrists[i][0], &wristIndeces[i][0]);
// 				polylines[i].getClosestPoint(wrists[i][1], &wristIndeces[i][1]);
// 			}

// 			hands[i].clear();			
// 			pts = polylines[i].getVertices();
// 			if(tipIndeces[i] > wristIndeces[i][0] && tipIndeces[i] < wristIndeces[i][1]) {
// 				for (int j = wristIndeces[i][0]; j < wristIndeces[i][1]; ++j)
// 				{
// 					hands[i].addVertex(pts[j]);
// 				}
// 				hands[i].addVertex(pts[wristIndeces[i][0]]);
// 			}
// 			else {
// 				for (int j = 0; j < wristIndeces[i][0]; ++j)
// 				{
// 					hands[i].addVertex(pts[j]);
// 				}
// 				for (int j = wristIndeces[i][1]; j < pts.size(); ++j)
// 				{
// 					hands[i].addVertex(pts[j]);
// 				}
// 			}
// 			// if(hands[i].getArea() <= MAX_HAND_SIZE && hands[i].getArea >= MIN_HAND_SIZE) {
// 			// }
// 			//cout << hands[i].getArea() << endl;
// 			return true;
// 		}	

// 		case 3: { //distance from tip method

// 		}
// 	}
// }