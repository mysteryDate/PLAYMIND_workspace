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
				endIndex.push_back(i);
		}
	}

	if(ends.size() == 2  and endIndex[0] > 0 and endIndex[1] < points.size()) {
		endsFound = true;
		findWrist();
	}
	else {
		endsFound = false;
		wristFound = false;
	}
		


}

void armBlob::findWrist() {

	if(endsFound == false) return;

	vector< ofPoint > &points = simplifiedBlob.getVertices();

	// we go back from the first one, forward from the second
	wrist[0] = points[endIndex[0] + 1];
	wrist[2] = points[endIndex[1] - 1];

	wristFound = true;

}
