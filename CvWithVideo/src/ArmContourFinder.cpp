#include "ArmContourFinder.h"

ArmContourFinder::ArmContourFinder() {

	bounds.push_back(19);
	bounds.push_back(50);
	bounds.push_back(605);
	bounds.push_back(478);

	tolerance = 9;

	simplifiedPolylines.resize(polylines.size());

}

void ArmContourFinder::update() {

	//To run every frame
	int size = polylines.size();

	simplifiedPolylines.resize(size);
	ends.resize(size);
	wrists.resize(size);
	tips.resize(size);

	endIndeces.resize(size);
	wristIndeces.resize(size);
	tipIndeces.resize(size);

	handFound.resize(size, false);
	hands.resize(size);

	for (int i = 0; i < polylines.size(); ++i)
	{
		simplifiedPolylines[i] = polylines[i];
		simplifiedPolylines[i].simplify(tolerance);
	}

}

void ArmContourFinder::updateArm(int i) {

	//For detected arms
	ofPoint * keypoints[] = {&ends[i][0], &ends[i][1], &tips[i], &wrists[i][0], &wrists[i][1]};

	ofPoint nearest[5];
	float distances[5];
	int MAX_DISTANCE = 500;
	
	for (int j = 0; j < 5; ++j)
	{
		unsigned int vertexIndex;
		simplifiedPolylines[i].getClosestPoint(*keypoints[j], &vertexIndex);
		nearest[j] = simplifiedPolylines[i][vertexIndex];
		distances[j] = ofDistSquared(nearest[j].x, nearest[j].y, keypoints[j]->x, keypoints[j]->y);
		if(distances[j] <= MAX_DISTANCE) {
			*keypoints[j] = nearest[j];
		}
		else
			handFound[i] = false;
	}
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

bool ArmContourFinder::findEnd(int i) {

	vector< ofPoint > pts = polylines[i].getVertices();
	vector< ofPoint > possibleEnds;
	vector< int > possibleEndIndeces;
	ends[i].clear();
	ends[i].resize(2);

	for (int i = 0; i < pts.size(); ++i)
	{
		if(pts[i].x == bounds[0] || pts[i].y <= bounds[1] + 2
			|| pts[i].x >= bounds[2] - 2 || pts[i].y >=  bounds[3] - 2) {
			possibleEnds.push_back(pts[i]);
		}
	}
	if(possibleEnds.size() >= 2) {
		ends[i][0] = possibleEnds[0];
		ends[i][1] = possibleEnds.back();
		polylines[i].getClosestPoint(ends[i][0], &endIndeces[i][0]);
		polylines[i].getClosestPoint(ends[i][1], &endIndeces[i][1]);
		return true;
	}
	else return false; 
}

ofPoint ArmContourFinder::findTip(int i) {

	// //Which direction has more points on it?
	// int a = endIndeces[i][1] - endIndeces[i][0];
	// int b = polylines[i].size() - a;
	// vector< ofPoint > pts = polylines[i].getVertices();

	// if(a > b) {
	// 	tipIndeces[i] = (a - b) / 2;
	// }
	// else {
	// 	if( (b/2) + endIndeces[i][1] > pts.size()) {
	// 		tipIndeces[i] = ( (b/2) + endIndeces[i][1] ) - pts.size();
	// 	}
	// 	else {
	// 		tipIndeces[i] = (b/2) + endIndeces[i][1];
	// 	}
	// }
	// unsigned int fullIndex;
	// //tips[i] = simplifiedPolylines[i].getClosestPoint( pts[tipIndeces[i]] );
	// simplifiedPolylines[i].getClosestPoint( pts[tipIndeces[i]], &fullIndex );
	// tips[i] = simplifiedPolylines[i].getVertices()[fullIndex];

	//Create a line connecting the center of the base of the arm to the center of the bounding box
	ofPoint boxCenter = ofxCv::toOf(getCenter(i));
	ofPoint baseCenter = ofPoint((ends[i][0].x + ends[i][1].x)/2, (ends[i][0].y + ends[i][1].y)/2 );
	// Slope of the line
	float m = (boxCenter.y - baseCenter.y) / (boxCenter.x - baseCenter.x);
	float yn = 5000;	// New y coordinate (far off)
	if(boxCenter.y < baseCenter.y) 
		yn *= -1;
	float xn = (yn - baseCenter.y) / m + baseCenter.x;
	ofPoint mostDistant = ofPoint(xn, yn);

	return simplifiedPolylines[i].getClosestPoint(mostDistant);

}

bool ArmContourFinder::findWrist(int i) {

	int method = 2;
	wrists[i].resize(2);
	wristIndeces[i].resize(2);

	switch(method) {
		case 1: { //Pushback method
			int pushBack = 50; // Amount of verteces we go back for the wrist

			int a = tipIndeces[i] - pushBack;
			int b = tipIndeces[i] + pushBack;

			if(a < 0) {
				a += polylines[i].size();
			}
			if(b > polylines[i].size()) {
				b -= polylines[i].size();
			}

			wristIndeces[i][0] = a;
			wristIndeces[i][1] = b;
			wrists[i][0] = polylines[i].getVertices()[a];
			wrists[i][1] = polylines[i].getVertices()[b];
			break; 
		}

		case 2: {// Next simplified vertex method
			//simplifiedPolylines[i] = polylines[i];
			//simplifiedPolylines[i].simplify(tolerance);
			vector< ofPoint > pts = simplifiedPolylines[i].getVertices();

			unsigned int nearestEnds[2];
			simplifiedPolylines[i].getClosestPoint(ends[i][0], &nearestEnds[0]);
			simplifiedPolylines[i].getClosestPoint(ends[i][1], &nearestEnds[1]);

			unsigned int nearestTip;
			simplifiedPolylines[i].getClosestPoint(tips[i], &nearestTip);

			int a = nearestEnds[1] - nearestEnds[0];
			int b = pts.size() - a;

			if(a > b) {
				wrists[i][0] = pts[nearestEnds[0] + 1];
				wrists[i][1] = pts[nearestEnds[1] - 1];
				polylines[i].getClosestPoint(wrists[i][0], &wristIndeces[i][0]);
				polylines[i].getClosestPoint(wrists[i][1], &wristIndeces[i][1]);
			}
			else {
				wrists[i][0] = pts[nearestEnds[0] - 1];
				wrists[i][1] = pts[nearestEnds[1] + 1];
				polylines[i].getClosestPoint(wrists[i][0], &wristIndeces[i][0]);
				polylines[i].getClosestPoint(wrists[i][1], &wristIndeces[i][1]);
			}

			hands[i].clear();			
			pts = polylines[i].getVertices();
			if(tipIndeces[i] > wristIndeces[i][0] && tipIndeces[i] < wristIndeces[i][1]) {
				for (int j = wristIndeces[i][0]; j < wristIndeces[i][1]; ++j)
				{
					hands[i].addVertex(pts[j]);
				}
				hands[i].addVertex(pts[wristIndeces[i][0]]);
			}
			else {
				for (int j = 0; j < wristIndeces[i][0]; ++j)
				{
					hands[i].addVertex(pts[j]);
				}
				for (int j = wristIndeces[i][1]; j < pts.size(); ++j)
				{
					hands[i].addVertex(pts[j]);
				}
			}
			// if(hands[i].getArea() <= MAX_HAND_SIZE && hands[i].getArea >= MIN_HAND_SIZE) {
			// }
			//cout << hands[i].getArea() << endl;
			return true;
		}	

		case 3: { //distance from tip method

		}
	}
}