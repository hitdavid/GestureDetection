#pragma once

typedef struct MovementInfo {
	int x;
	int y;

	CTime timeStamp;
	double acceleration;
	double velocity;
	int orientation;

}MovementInfo;