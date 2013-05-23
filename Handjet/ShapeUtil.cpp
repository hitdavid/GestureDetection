#include "stdafx.h"
#include "ShapeUtil.h"

using namespace std;

int Handjet::ShapeUtil::calcLength(CvPoint p1, CvPoint p2) {
	return sqrt((double)abs(p1.x - p2.x)*abs(p1.x - p2.x) + (double)abs(p1.y - p2.y)*abs(p1.y - p2.y));
}

double Handjet::ShapeUtil::calcAngle(int adjacentSideLen1, int adjacentSideLen2, int onSideLen) {
	double abc2 = (adjacentSideLen1*adjacentSideLen1 + adjacentSideLen2*adjacentSideLen2 - onSideLen*onSideLen);
	return acos(abc2 / (2 * adjacentSideLen1 * adjacentSideLen2)) * 180 / Pi;
}

double Handjet::ShapeUtil::calcAngle(CvPoint p1, CvPoint p2, CvPoint turingPoint) {
	return 0.0;
}

double Handjet::ShapeUtil::calcOrientation(double depthAngle, CvPoint endPoint, CvPoint turingPoint) {
	double a = atan2f((endPoint.y - turingPoint.y), (endPoint.x - turingPoint.x)) * 180 / Pi;
	return -(a + depthAngle / 2);
}




