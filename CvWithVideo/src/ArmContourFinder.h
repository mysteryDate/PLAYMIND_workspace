#define MAXIMUM_DISTANCE 10

#include "ContourFinder.h"
#include "ofMain.h"

class ArmContourFinder : public ofxCv::ContourFinder
{
public:

	ArmContourFinder();

	vector< vector< ofPoint > > ends;
	vector< ofPoint > tips;
	vector< vector< ofPoint > > wrists;

	// Not the best way to do this, but it'll do for now
	vector< vector < unsigned int > > endIndeces;
	vector< unsigned int > tipIndeces;
	vector< vector < unsigned int > > wristIndeces;

	vector< bool > handFound;

	vector< ofPolyline > simplifiedPolylines;
	vector< ofPolyline > hands;

	void update();
	void updateArm(int i);

	// void findEnds();
	bool findEnd(int i);
	bool findTip(int i);
	bool findWrist(int i);

	void setBounds(int xMin, int yMin, int xMax, int yMax );
	void setTolerance(float tolerance);

	vector< int >& getBounds();
	int* getTolerance();


private:

	vector< int > bounds;

	float tolerance;

};