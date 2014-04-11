#include "ContourFinder.h"
#include "ofMain.h"

class ArmContourFinder : public ofxCv::ContourFinder
{
public:

	ArmContourFinder();

	vector< vector< ofPoint > > ends;
	vector< ofPoint > wrists[2];
	vector< ofPolyline > simplifiedPolylines;

	void findEnds();

	void setBounds(int xMin, int yMin, int xMax, int yMax );
	void setTolerance(float tolerance);

	vector< int >& getBounds();
	int* getTolerance();


private:

	vector< int > bounds;

	float tolerance;

};