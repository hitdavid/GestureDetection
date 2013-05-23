#pragma once

#include "TransformLog.h"
#include "CandidateObject.h"

using namespace std;

namespace Handjet{
	class TracingTableItem{

	public: 
		CandidateObject* m_candidate;
		TransformLog m_log;
		__time64_t m_lastUpdate;
	};
}

