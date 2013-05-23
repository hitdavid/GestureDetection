#pragma once

#include "CandidateObject.h"
#include "PositionTransform.h"

using namespace std;

namespace Handjet {
	class Transform {
		private: 
			long m_duration;
			long m_timeStamp;
			double m_velocity;
			double m_acceleration;
			double m_orientation;
	};
}