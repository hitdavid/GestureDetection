#pragma once

#include "CandidateObject.h"
#include "TracingTableItem.h"
#include "TransformAnalyzer.h"
#include "PositionSmoothFilter.h"

namespace Handjet{
	class CandidateTracer
	{
	private:

		bool isLeftDown;
		bool isDraging;
		clock_t  lastLeftDownTime;
		clock_t  lastLeftUpTime;

		clock_t lastGuestureTime;

		CvPoint mode1LastPoint;

		std::list<CandidateObject*>* m_lastFrameCandidates;
		std::list<CandidateObject*>* m_thisFrameCandidates;
		std::list<TracingTableItem*>* m_tracingTable;
		TransformAnalyzer* analyzer;
		PositionSmoothFilter* m_filter;

	public:
		CandidateTracer(void);
		~CandidateTracer(void);

		void updateCandidates(std::list<CandidateObject*>* l);
	};
}

