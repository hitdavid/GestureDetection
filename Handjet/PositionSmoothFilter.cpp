#include "StdAfx.h"
#include "PositionSmoothFilter.h"
#include "ShapeUtil.h"

using namespace Handjet;

PositionSmoothFilter::PositionSmoothFilter(void) {
	history = new std::list<MovementInfo*>();
	radius = 1000;
	listLength = 500;
}


PositionSmoothFilter::~PositionSmoothFilter(void) {

	MovementInfo* mi = NULL;
	while ( history->size() > 0)
	{
		mi = history->back();
		//hc->clearAllDefectBlocks();
		free(mi);
		mi = NULL;
		history->pop_back();
	}
	delete history;
}

CvPoint PositionSmoothFilter::retriveSmoothedPosition() {
	return doRetriveSmoothedPosition();
}

CvPoint PositionSmoothFilter::doRetriveSmoothedPosition() {

	//int s = ;
	if(history->size() > 0) {
		MovementInfo* mi = history->back();
		int tx = 0, ty = 0;

		for (std::list<MovementInfo*>::iterator it = history->begin(); it != history->end(); ++ it)
		{
			int distance = ShapeUtil::calcLength(cvPoint((*it)->x, (*it)->y), cvPoint(mi->x, mi->y));
			if(distance < radius) {
				if(tx != 0 || ty != 0) {
					tx = tx * 2 / 3 + (*it)->x / 3;
					ty = ty * 2 / 3 + (*it)->y / 3;
				}
				else {
					tx = (*it)->x;
					ty = (*it)->y;
				}
			}
		}

		return cvPoint(tx, ty);
	}
	else {
		cvPoint(0, 0);
	}
}


MovementInfo* PositionSmoothFilter::addOriginalPosition(int x, int y) {
	MovementInfo* newInfo = new MovementInfo();
	newInfo->x = x;
	newInfo->y = y;


	newInfo->timeStamp = CTime::GetTickCount();
	int s = history->size();

	if(s > 0) {
		MovementInfo* lastInfo = history->back();


		int len = ShapeUtil::calcLength(cvPoint(lastInfo->x, lastInfo->y), cvPoint(x, y));
		CTimeSpan dt = newInfo->timeStamp - lastInfo->timeStamp;
		__time64_t time = dt.GetTimeSpan() ? dt.GetTimeSpan() : 1;
		newInfo->velocity = len / time;
		newInfo->acceleration = 2*(len - lastInfo->velocity * time) / time * time;
		newInfo->orientation = atan2f((y - lastInfo->y), (x - lastInfo->x)) * 180 / Pi;
		while(history->size() > listLength) {
			history->pop_front();
		}
	}
	else {
		newInfo->acceleration = 0;
		newInfo->velocity = 0;
		newInfo->orientation = 0;

	}

	history->push_back(newInfo);
	return newInfo;
}

MovementInfo* PositionSmoothFilter::addOriginalPosition(CvPoint p) {
	return addOriginalPosition(p.x, p.y);
}

void PositionSmoothFilter::setFilterRadius(int r) {
	radius = r;
}

void PositionSmoothFilter::setFilterListLength(int len) {
	listLength = len;
}