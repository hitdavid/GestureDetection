#include "StdAfx.h"
#include "CandidateTracer.h"
#include "MovementInfo.h"
#include "MousePointerHelper.h"
#include "ImageProcessor.h"
#include "resource.h"
#include <ctime>


Handjet::CandidateTracer::CandidateTracer(void) {
	m_lastFrameCandidates = new std::list<CandidateObject*>();
	m_thisFrameCandidates = new std::list<CandidateObject*>();
	m_tracingTable = new std::list<TracingTableItem*>();
	analyzer = new TransformAnalyzer();
	m_filter = new PositionSmoothFilter();
	isLeftDown = false;
	isRightDown = false;
	lastLeftDownTime = clock();
	lastLeftUpTime = clock();

	lastRightDownTime = clock();
	lastRightUpTime = clock();

	lastGuestureTime = clock();
	isDraging = false;
	mode1LastPoint = cvPoint(0, 0);
	frameWithoutHand = 0;
}


Handjet::CandidateTracer::~CandidateTracer(void)
{
	delete m_filter;
}

void Handjet::CandidateTracer::updateCandidates(std::list<CandidateObject*>* l) {
	CandidateObject* co = NULL;
	while ( m_lastFrameCandidates->size() > 0)
	{
		co = m_lastFrameCandidates->back();
		//hc->clearAllDefectBlocks();
		delete co;
		co = NULL;
		m_lastFrameCandidates->pop_back();
	}
	//delete m_lastFrameCandidates;
	for(std::list<CandidateObject*>::iterator it=m_thisFrameCandidates->begin(); it != m_thisFrameCandidates->end(); ++it) {
		m_lastFrameCandidates->push_back(*it);
	}

	m_thisFrameCandidates->clear();
	for(std::list<CandidateObject*>::iterator it=l->begin(); it != l->end(); ++it) {
		m_thisFrameCandidates->push_back(*it);
	}

	CandidateObject* hand = analyzer->analyze(m_lastFrameCandidates, m_thisFrameCandidates, m_tracingTable);

	if(hand != NULL) {
	
		int mode = ImageProcessor::Instance()->getRunMode();
		if(mode == 1) {
			//ppt mode
			if(mode1LastPoint.x == 0 && mode1LastPoint.y == 0) {
				mode1LastPoint = hand->getValidPosition();
			}
			else if(frameWithoutHand != 0){
				//if(!CandidateObject::haveJudgeResultBit(hand->getShape(), CandidateObject::MAYBE_OPEN_PALM)) {
					
					clock_t  cts = clock() - lastGuestureTime;
					float timediff_sec = ((float)cts);// / CLOCKS_PER_SEC;
					
					CvPoint newPoint = hand->getValidPosition();
					if((newPoint.x - mode1LastPoint.x < -70) ||
						(newPoint.y - mode1LastPoint.y < -70)) {
						if(timediff_sec > 1000) {
							SetCursor(AfxGetApp()->LoadCursor(IDC_SWIPE_L));
							MousePointerHelper::Instance()->prevSlideAction();
							lastGuestureTime = clock();
							return;
						}
					}
					else if((newPoint.x - mode1LastPoint.x > 70) ||
							(newPoint.y - mode1LastPoint.y > 70)){
						if(timediff_sec > 1000) {
							SetCursor(AfxGetApp()->LoadCursor(IDC_SWIPE_R));
							MousePointerHelper::Instance()->nextSlideAction();
							lastGuestureTime = clock();
							return;
						}
					}
				//}
				mode1LastPoint = hand->getValidPosition();
			}
			else {
				mode1LastPoint = hand->getValidPosition();
			}
			//return;
		}

		if(CandidateObject::haveJudgeResultBit(hand->getShape(), CandidateObject::MAYBE_FIST) && hand->recheckIfFist()) {
			if(!isLeftDown) {
				isLeftDown = true;
				lastLeftDownTime = clock();
				SetCursor(LoadCursor(NULL, IDC_HAND));
			}
			else {
				clock_t  cts = clock() - lastLeftDownTime;
				float timediff_sec = ((float)cts);// / CLOCKS_PER_SEC;
				if(timediff_sec > 3000 && isDraging == false) {
					if(mode == 0) {
						MousePointerHelper::Instance()->dragBegin();
						SetCursor(LoadCursor(NULL, IDC_WAIT));
					}
					isDraging = true;
				}
				else if(isDraging) {
					
					clock_t  cts = clock() - lastLeftDownTime;
					float timediff_sec = ((float)cts);// / CLOCKS_PER_SEC;
					if(timediff_sec > 100) {
						lastLeftDownTime = clock();
						MovementInfo* i = m_filter->addOriginalPosition(hand->getValidPosition());
						CvPoint p = m_filter->retriveSmoothedPosition();
						MousePointerHelper::Instance()->MoveTo(p);
					}
					
				}
				else {
					//MovementInfo* i = m_filter->addOriginalPosition(hand->getValidPosition());
					hand->getValidPosition();
					//CvPoint p = m_filter->retriveSmoothedPosition();
					//MousePointerHelper::Instance()->MoveTo(p);
				}
			}
		}
		else if(CandidateObject::haveJudgeResultBit(hand->getShape(), CandidateObject::MAYBE_FIST) && !hand->recheckIfFist()) {
			if(CandidateObject::haveJudgeResultBit(hand->getShape(), CandidateObject::MAYBE_FOUR)) {
				if(!isRightDown && mode == 0) {
					isRightDown = true;
					lastRightDownTime = clock();
				}
			}
		}
		else if(CandidateObject::haveJudgeResultBit(hand->getShape(), CandidateObject::MAYBE_OPEN_PALM)) {
			if(isLeftDown && frameWithoutHand != 0) {
				isLeftDown = false;
				if(isDraging) {
					clock_t  cts = clock() - lastLeftUpTime;
					float timediff_sec = ((float)cts);// / CLOCKS_PER_SEC;
					if(timediff_sec > 1000) {
						if(mode == 0) {
							MousePointerHelper::Instance()->dragRelease();
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}
						isDraging = false;
					}
					
				}
				else {
					clock_t cts = clock() - lastLeftDownTime;
					float timediff_sec = ((float)cts);// / CLOCKS_PER_SEC;
					if(timediff_sec > 2000) {
						if(mode == 0) {
							MousePointerHelper::Instance()->leftDBClick();
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}
						else if(mode == 1 && timediff_sec < 5000) {
							MousePointerHelper::Instance()->leftClick();
						}
					}else if(timediff_sec > 500) {
						if(mode == 0) {
							MousePointerHelper::Instance()->leftClick();
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}
					}
				}
			}
			else if(isRightDown && frameWithoutHand != 0) {
				
				
				clock_t  cts = clock() - lastRightDownTime;
				float timediff_sec = ((float)cts);// / CLOCKS_PER_SEC;
				if(timediff_sec > 1000 && mode == 0) {
					MousePointerHelper::Instance()->rightClick();
					isRightDown = false;
					lastRightDownTime = clock();
				}
				
			}
			else {
				frameWithoutHand++;
				if(CandidateObject::haveJudgeResultBit(hand->getShape(), CandidateObject::MAYBE_OPEN_PALM)) {
					MovementInfo* i = m_filter->addOriginalPosition(hand->getValidPosition());
					CvPoint p = m_filter->retriveSmoothedPosition();
					MousePointerHelper::Instance()->MoveTo(p);
				}
			}
		}

	}
	else {
		frameWithoutHand = 0;
	}
	
}