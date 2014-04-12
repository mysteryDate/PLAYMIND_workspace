#include "ArmContourFinder.h"

ArmContourFinder::ArmContourFinder() {

	bounds.push_back(19);
	bounds.push_back(50);
	bounds.push_back(605);
	bounds.push_back(478);

	tolerance = 9;

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

	ends.clear();
	ends.resize(polylines.size() );
	for (int i = 0; i < polylines.size(); ++i)
	{
        ofPolyline line = polylines[i].getVertices();
        //line.simplify(tolerance);
		vector< ofPoint > pts = line.getVertices();
		vector< ofPoint > possibleEnds;
		ends[i].clear();
		ends[i].resize(2);
		for (int j = 0; j < pts.size(); ++j)
		{
			if(pts[j].x == bounds[0] || pts[j].y == bounds[1]
				|| pts[j].x == bounds[2] || pts[j].y == bounds[3]) {
				possibleEnds.push_back(pts[j]);
			}
		}
		if(possibleEnds.size() >= 2 ) {
			//Set the actual ends the first and the last ones found
			ends[i][0] = possibleEnds[0];
			ends[i][1] = possibleEnds.back();
		}
	}

}