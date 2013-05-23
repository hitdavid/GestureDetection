#pragma once

#include "CvHeader.h"
#include "MovementInfo.h"

namespace Handjet {
	class PositionSmoothFilter
	{
	private:
		bool enabled;
		std::list<MovementInfo*> *history;

		int radius;
		int listLength;

		CvPoint doRetriveSmoothedPosition();

	public:
		PositionSmoothFilter(void);
		~PositionSmoothFilter(void);

		CvPoint retriveSmoothedPosition();

		MovementInfo* addOriginalPosition(int x, int y);
		MovementInfo* addOriginalPosition(CvPoint p);

		void setFilterRadius(int r);
		void setFilterListLength(int len);
	};
}

