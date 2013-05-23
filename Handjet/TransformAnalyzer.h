#pragma once
#include "TracingTableItem.h"
#include "CandidateObject.h"
#include "HandCandidate.h"

using namespace std;
using namespace Handjet;

namespace Handjet {
	class TransformAnalyzer {
	public:
		TransformAnalyzer();
		~TransformAnalyzer();

	private:
		CandidateObject* hand;
		list<TracingTableItem*>* m_tracingTable;

	private:
		CandidateObject* findObjectInLastFrame(CandidateObject* candidate, list<CandidateObject*>* lastFrame);
		TracingTableItem* findTracingTableItem(int token);

		int calcShapeScore(CandidateObject* object, CandidateObject* candidate);
		int calcPositionScore(CandidateObject* object, CandidateObject* candidate);

	public:
		CandidateObject* analyze(list<CandidateObject*>* lastFrame, list<CandidateObject*>* thisFrame, list<TracingTableItem*>* tracingTable);
		CandidateObject* findHandCandidate(list<CandidateObject*>* candidates);
	};
}