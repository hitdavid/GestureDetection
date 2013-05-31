#include "StdAfx.h"
#include "MousePointerHelper.h"
#include "resource.h"

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

	HWND hWnd = WindowFromPoint(CPoint(0, 0));
	HDC hDC = GetWindowDC(hWnd);
	backupMemDC = CreateCompatibleDC(hDC);

	firstDraw = true;
}


Handjet::MousePointerHelper::~MousePointerHelper(void)
{
	HWND hWnd = WindowFromPoint(CPoint(0, 0));
	::DeleteDC(backupMemDC);
	::ReleaseDC(hWnd, backupMemDC);
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
	//drawCursor(x, y);
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


void Handjet::MousePointerHelper::drawCursor(int x, int y)
{
	//HWND hWnd = WindowFromPoint(CPoint(x, y));
	//if(!firstDraw)
	//	SetCapture(hWnd);
	//HWND hWnd = WindowFromPoint(CPoint(x, y));
	////创建屏幕DC
	//HDC hDC = GetWindowDC(hWnd);
	////创建内存DC
	//HDC hMemDC = CreateCompatibleDC(hDC);
	//backupMemDC = CreateCompatibleDC(hDC);

	//CBitmap hBitmap;
	//hBitmap.LoadBitmap(IDB_BITMAP1);
	//HBITMAP oldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	//if(!firstDraw) {
	//	::TransparentBlt(hDC, lastx, lasty, 32, 32, backupMemDC, 0, 0, 32, 32, RGB(255, 255, 255));
	//}

	//
	////backup
	//::TransparentBlt(backupMemDC, 0, 0, 32, 32, hDC, x, y, 32, 32, RGB(255, 255, 255));

	//::TransparentBlt(hDC, x, y, 32, 32, hMemDC, 0, 0, 32, 32, RGB(255, 255, 255));
	//lastx = x;
	//lasty = y;
	//firstDraw = false;

	//::SelectObject(hMemDC, oldBitmap);
	//::DeleteObject(hBitmap);
	////删除内存DC
	//::DeleteDC(hMemDC);
	////释放屏幕DC
	//::ReleaseDC(hWnd, hDC);
}