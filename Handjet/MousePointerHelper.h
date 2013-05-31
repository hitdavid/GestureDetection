#pragma once

#include "CvHeader.h"

namespace Handjet {
	class MousePointerHelper
	{
	private:
		MousePointerHelper(void);
		CvPoint originalPoint;
		int capturedWidth;
		int capturedHeight;

		int screenWidth;
		int screenHeight;
		HDC backupMemDC;

		int lastx;
		int lasty;

		bool firstDraw;

	public:

		const static unsigned int WHELL_SCROLL_UP = 0;
		const static unsigned int WHELL_SCROLL_DOWN = 1;


		static MousePointerHelper* Instance();
		~MousePointerHelper(void);

		void MoveTo(CvPoint p);
		CvPoint getPos();

		void leftClick();
		void leftDBClick();

		void rightClick();

		void wheelClick();

		void dragBegin();
		void dragRelease();

		void prevSlideAction(); //page up
		void nextSlideAction(); //page down

		void wheelScroll(unsigned int direction, unsigned int delta);
		void drawCursor(int x, int y);
	};
}
