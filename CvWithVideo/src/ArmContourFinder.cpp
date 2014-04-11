#include "ArmContourFinder.h"

ArmContourFinder::ArmContourFinder() {

	bounds.push_back(19);
	bounds.push_back(50);
	bounds.push_back(605);
	bounds.push_back(478);

	tolerance = 1;

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

void ArmContourFinder::findEnds() {

	for (int i = 0; i < polylines.size(); ++i)
	{
		vector< ofPoint > pts = polylines[i].getVertices();
		for (int j = 0; j < pts.size(); ++j)
		{
			vector< ofPoint > possibleEnds;
			if(pts[i].x <= bounds[0] || pts[i].y <= bounds[1] 
				|| pts[i].x >= bounds[2] || pts[i].y >= bounds[3]) {
				possibleEnds.push_back(pts[i]);
			}
			if(possibleEnds.size() >= 2 ) {
				//Set the actual ends the first and the last ones found
				ends[i][0] = possibleEnds[0];
				ends[i][1] = possibleEnds.back();
			}
		}
	}

}