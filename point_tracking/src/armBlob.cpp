#include "armBlob.h"

armBlob::armBlob(ofxCvBlob blob, float tolerance) {

	pts = blob.pts;
	simplifiedBlob = ofPolyline( pts );
    simplifiedBlob.simplify(tolerance);

    wristFound = false;
    endsFound = false;
    padding = 10;

}

void armBlob::findEnds(int minx, int miny, int maxx, int maxy) {

	vector< ofPoint > &points = simplifiedBlob.getVertices();
	ends.clear();
	for (int i = 0; i < points.size(); ++i)
	{
		if(points[i].x <= minx + padding or points[i].x >= maxx - padding
			or points[i].y <= miny + padding or points[i].y >= maxy - padding) {
				ends.push_back(points[i]);	
		}
	}

}

void armBlob::findWrist() {

}
