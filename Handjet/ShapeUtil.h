#pragma once

#include "CvHeader.h"

using namespace std;

#define Pi 3.1415927

namespace Handjet {
	class ShapeUtil {
		
	public: 
		static int calcLength(CvPoint p1, CvPoint p2);
		static double calcAngle(int adjacentSideLen1, int adjacentSideLen2, int onSideLen);
		static double calcAngle(CvPoint p1, CvPoint p2, CvPoint turingPoint);
		static double calcOrientation(double depthAngle, CvPoint endPoint, CvPoint turingPoint);
	};
}