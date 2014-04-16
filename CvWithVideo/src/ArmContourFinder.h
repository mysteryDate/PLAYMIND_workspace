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

	// Matrix of how many skips have happened, first dimension is id, then end1, end2, tip
	vector< vector< int > > skippedFrames;

	// Not the best way to do this, but it'll do for now
	vector< vector < unsigned int > > endIndeces;
	vector< unsigned int > tipIndeces;
	vector< vector < unsigned int > > wristIndeces;

	vector< bool > handFound;

	vector< ofPolyline > simplifiedPolylines;
	vector< ofPolyline > hands;

	void update();
	void updateArm(int n);

	// void findEnds();
	vector< ofPoint > findEnds(int n);
	ofPoint findTip(int n);
	vector< ofPoint > findWrist(int n, ofPoint newTip);
	void findHand(int n);

	void setBounds(int xMin, int yMin, int xMax, int yMax );
	void setTolerance(float tolerance);

	vector< int >& getBounds();
	int* getTolerance();
	int MIN_HAND_SIZE;
	int MAX_HAND_SIZE;
	int SMOOTHING_RATE;


private:

	vector< int > bounds;

	float tolerance;

	int MAX_MOVEMENT_DISTANCE;
	int SURVIVAL_FRAMES;

};