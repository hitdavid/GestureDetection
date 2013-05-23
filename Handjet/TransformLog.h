#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __TransformLog_h__
#define __TransformLog_h__

// #include "Transform.h"
// #include "CandidateTracer.h"

namespace Handjet
{
	class Transform;
	class CandidateTracer;
	class TransformLog;
}

namespace Handjet
{
	class TransformLog
	{
		private: Handjet::Transform* m_logData;
		private: int m_objectToken;
		public: Handjet::CandidateTracer* m_unnamed_CandidateTracer_;
		public: Handjet::Transform* m_unnamed_Transform_;

		public: void log(Handjet::Transform* aT);
	};
}

#endif
