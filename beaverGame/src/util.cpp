#include "util.h"

vector< ofPolyline > util::transform(vector< ofPolyline > input, int dx, int dy, int z, int r = 0)
{
	vector< ofPolyline > output;		
	for (int i = 0; i < input.size(); ++i)
	{
		ofPolyline line = input[i];
		ofPolyline tLine;
		for (int j = 0; j < line.size(); ++j)
		{
			ofPoint pt = line[j];
			float tx = pt.x * z + x;
			float ty = pt.y * z + x;
			tLine.addVertex(tx, ty);
		}
		output.push_back(tLine);
	}
	return output;
}

vector< ofPoint > util::transform(vector< ofPoint > input, int dx, int dy, int z, int r = 0)
{
	vector< ofPoint > output;		
	for (int i = 0; i < input.size(); ++i)
	{
		ofPoint pt = input[i];
		float tx = pt.x * z + x;
		float ty = pt.y * z + x;
		output.push_back(ofPoint(tx, ty));
	}
	return output;
}

ofPoint util::transform(ofPoint input, int dx, int dy, int z, int r = 0)
{
	ofPoint output;		
	float tx = input.x * z + x;
	float ty = input.y * z + x;
	output = ofPoint(tx, ty);
	return output;
}