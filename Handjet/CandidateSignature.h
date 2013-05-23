#pragma once


#define BIG_DEFECT_THRESHOLD 0.6


typedef struct CandidateSignature {
	//int x;
	//int y;
	//int width;
	//int height;
	double rectArea;
	int totalDefects;
	double defectArea;
	double outerObjectArea;
	int defectsInThreshold;
	double defectOrientation;
}CandidateSignature;