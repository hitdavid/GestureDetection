#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __ShapeTransform_h__
#define __ShapeTransform_h__

// #include "Transform.h"

namespace Handjet
{
	class Transform;
	class ShapeTransform;
}

namespace Handjet
{
	class ShapeTransform
	{
		public: Handjet::Transform* m_unnamed_Transform_;
	};
}

#endif
