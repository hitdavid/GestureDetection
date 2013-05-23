#include "StdAfx.h"
#include "MousePointerHelper.h"

static Handjet::MousePointerHelper* instance;

Handjet::MousePointerHelper* Handjet::MousePointerHelper::Instance() {
	if(instance == NULL) {
		instance = new MousePointerHelper();
	}
	return instance;
}

Handjet::MousePointerHelper::MousePointerHelper(void) {
	this->originalPoint = cvPoint(150, 150);
	this->capturedWidth = 340;
	this->capturedHeight = 180;
	this->screenWidth = GetSystemMetrics(SM_CXSCREEN);  
	this->screenHeight = GetSystemMetrics(SM_CYSCREEN);  
}


Handjet::MousePointerHelper::~MousePointerHelper(void)
{
}

void Handjet::MousePointerHelper::MoveTo(CvPoint p) {
	int x, y;

	if(p.x >= originalPoint.x && p.x <= originalPoint.x + capturedWidth) {
		x = (p.x - originalPoint.x) * screenWidth / capturedWidth; 
	}
	else if(p.x < originalPoint.x) {
		x = 0;
	}
	else {
		x = screenWidth;
	}

	if(p.y >= originalPoint.y && p.y <= originalPoint.y + capturedHeight) {
		y = (p.y - originalPoint.y) * screenHeight / capturedHeight; 
	}
	else if(p.y < originalPoint.y) {
		y = 0;
	}
	else {
		y = screenHeight;
	}

	SetCursorPos(x, y);
}

//CvPoint MousePointerHelper::getPos() {
//	return cvPoint(0, 0);
//}

void Handjet::MousePointerHelper::leftClick(){
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}
void Handjet::MousePointerHelper::leftDBClick(){
	leftClick();
	leftClick();
}

void Handjet::MousePointerHelper::rightClick(){
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

void Handjet::MousePointerHelper::wheelClick(){
	mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
}

void Handjet::MousePointerHelper::dragBegin() {
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
}

void Handjet::MousePointerHelper::dragRelease() {
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void Handjet::MousePointerHelper::prevSlideAction() {
	keybd_event(VK_PRIOR, 0, 0, 0);
}

void Handjet::MousePointerHelper::nextSlideAction() {
	keybd_event(VK_NEXT, 0, 0, 0);
}

void Handjet::MousePointerHelper::wheelScroll(unsigned int direction, unsigned int delta){

	int d;
	if(direction == MousePointerHelper::WHELL_SCROLL_UP) {
		d = 120;
	}
	else {
		d = -120;
	}
	mouse_event (MOUSEEVENTF_WHEEL , 0, 0, d, 0 );
}
