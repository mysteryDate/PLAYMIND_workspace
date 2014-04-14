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

	for (int i = 0; i < polylines.size(); ++i)
	{
		simplifiedPolylines[i] = polylines[i];
		simplifiedPolylines[i].simplify(tolerance);
	}

}

void ArmContourFinder::updateArm(int i) {

	//For detected arms
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

	vector< ofPoint > pts = simplifiedPolylines[i].getVertices();
	vector< ofPoint > possibleEnds;
	vector< int > possibleEndIndeces;
	ends[i].clear();
	ends[i].resize(2);

	for (int i = 0; i < pts.size(); ++i)
	{
		if(pts[i].x == bounds[0] || pts[i].y == bounds[1]
			|| pts[i].x >= bounds[2] - 5 || pts[i].y >=  bounds[3] - 5) {
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

bool ArmContourFinder::findTip(int i) {

	//Which direction has more points on it?
	int a = endIndeces[i][1] - endIndeces[i][0];
	int b = polylines[i].size() - a;
	vector< ofPoint > pts = polylines[i].getVertices();

	if(a > b) {
		tipIndeces[i] = (a - b) / 2;
	}
	else {
		if( (b/2) + endIndeces[i][1] > pts.size()) {
			tipIndeces[i] = ( (b/2) + endIndeces[i][1] ) - pts.size();
		}
		else {
			tipIndeces[i] = (b/2) + endIndeces[i][1];
		}
	}
	tips[i] = simplifiedPolylines[i].getClosestPoint(pts[tipIndeces[i]]);
	return true;


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
			}
			else {
				wrists[i][0] = pts[nearestEnds[0] - 1];
				wrists[i][1] = pts[nearestEnds[1] + 1];
			}

			return true;
			break; 
		}
		
	}



}

// void ArmContourFinder::findArms() {

// 	for (int i = 0; i < polylines.size(); ++i)
// 	{
// 		// We don't have the right ends
// 		if(ends[i].size() != 2) {
// 			//find ends, if successful
// 			//find tip/wrists
// 			//else continue
// 		}
// 		else {
// 			// We have one tip
// 			if(tip[i].size() != 1) {

// 			}
// 			if(wrists[i].size() != 2) {}
// 		}
// 	}

// }