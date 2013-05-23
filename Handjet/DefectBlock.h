#pragma once

#include "CvHeader.h"

#define NEIGHBOR_DISTANCE 20

typedef struct DefectBlock {

	int block_id;

	CvPoint start;
	CvPoint end;
	CvPoint depth;

	double start_end_len;
	double depth_end_len;
	double start_depth_len;

	double area;

	double depthAngle;
	int depthLength;

	double orientation;

	int neighbors;

	int leftNeighborId;
	int rightNeighborId;

	int judgedValue;

}DefectBlock;






