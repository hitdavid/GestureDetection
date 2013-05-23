#include "StdAfx.h"
#include "CandidateObject.h"
#include "DefectBlock.h"
#include "ShapeUtil.h"
#include "ImageProcessor.h"

using namespace std;

Handjet::CandidateObject::CandidateObject(void) {
	m_DefectBlocks = new std::list<DefectBlock*>();
	m_currentShape = 0;
	m_Position = cvPoint(0, 0);
}


Handjet::CandidateObject::~CandidateObject(void) {
	DefectBlock* defectb = NULL;
	while ( m_DefectBlocks->size() > 0)
	{
		defectb = m_DefectBlocks->back();
		//hc->clearAllDefectBlocks();
		free(defectb);
		defectb = NULL;
		m_DefectBlocks->pop_back();
	}
	delete m_DefectBlocks;
}

CvPoint Handjet::CandidateObject::getCenter() {

	m_center.x = m_rect.x + m_rect.width / 2;
	m_center.y = m_rect.y + m_rect.height / 2;
	return m_center;
}

CvPoint Handjet::CandidateObject::getFistPosition() {

	int toppest = 481;
	DefectBlock* topBlock = NULL;
	for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it) {
		//找到顶部的block，然后求其邻居们的坐标平均值
		if((*it)->start.y < toppest) {
			toppest = (*it)->start.y;
			topBlock = *it;
		}
	}

	std::list<DefectBlock*>* neighbors = new std::list<DefectBlock*>();
	neighbors->push_back(topBlock);
	
	bool alreadyIn = false;

	for (int i = 0; i < neighbors->size(); i++) {
		for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it) {
			if(ShapeUtil::calcLength((*it)->start, topBlock->end) < 50 ||
				ShapeUtil::calcLength((*it)->end, topBlock->start) < 50) {

				for (std::list<DefectBlock*>::iterator nit = neighbors->begin(); nit != neighbors->end(); ++ nit) {
					if(*it == *nit) {
						alreadyIn = true;
					}
				}
				if(!alreadyIn) {
					if((*it)->area <= 200) {
						neighbors->push_back(*it);
					}
				}
				else {
					alreadyIn = false;
				}
			}
		}
	}

	int x = 0, y = 0;
	int num = 0;
	for (std::list<DefectBlock*>::iterator it = neighbors->begin(); it != neighbors->end(); ++ it) {
		//找到顶部的block，然后求其邻居们的坐标平均值
		x += (*it)->depth.x;
		y += (*it)->depth.y;
		num ++;

		IplImage* m_displayImage = ImageProcessor::Instance()->getDisplayedImage();
		cvCircle(m_displayImage, (*it)->start, 10, CV_RGB(255,0,0), 1, 8 ,0);
		cvCircle(m_displayImage, (*it)->end, 10, CV_RGB(0,255,0), 1, 8 ,0);
		cvCircle(m_displayImage, (*it)->depth, 10, CV_RGB(0,0,255), 1, 8 ,0);
		cvLine(m_displayImage, (*it)->start, (*it)->end, CV_RGB(0,0,255), 3, 8, 0);
		cvLine(m_displayImage, (*it)->start, (*it)->depth, CV_RGB(0,255,0), 3, 8, 0);
		cvLine(m_displayImage, (*it)->depth, (*it)->end, CV_RGB(255,0,0), 3, 8, 0);
	}

	delete neighbors;

	return cvPoint(x / num , y/ num);

}

CvPoint Handjet::CandidateObject::getValidPosition() {

	if(haveJudgeResultBit(m_currentShape, MAYBE_FIST)) {
		return getFistPosition();
	}

	double lastOrientation = 0.0;
	std::list<DefectBlock*>* fingerBlocks = new std::list<DefectBlock*>();

	for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it)
	{
		DefectBlock* block = *it;
		if(block->depthAngle <= 100) {
			//可能是手指之间的区块
			if(block->area > m_signature.defectArea * 0.05 && m_signature.defectArea > m_signature.rectArea * 0.1) {
				//面积足够大
				if(block->start_end_len < (block->depth_end_len + block->start_depth_len) * 5 / 6) {
					
					if(lastOrientation > 0.01 || lastOrientation < -0.01) { 
						if(abs((*it)->orientation - lastOrientation) < 80) {
							//result = MAYBE_HEAD | MAYBE_HEAD_WITH_NECK;
							if((*it)->depthAngle <=100) {
								fingerBlocks->push_back(block);

								IplImage* m_displayImage = ImageProcessor::Instance()->getDisplayedImage();
								cvCircle(m_displayImage, (*it)->start, 10, CV_RGB(255,0,0), 1, 8 ,0);
								cvCircle(m_displayImage, (*it)->end, 10, CV_RGB(0,255,0), 1, 8 ,0);
								cvCircle(m_displayImage, (*it)->depth, 10, CV_RGB(0,0,255), 1, 8 ,0);
								cvLine(m_displayImage, (*it)->start, (*it)->end, CV_RGB(0,0,255), 3, 8, 0);
								cvLine(m_displayImage, (*it)->start, (*it)->depth, CV_RGB(0,255,0), 3, 8, 0);
								cvLine(m_displayImage, (*it)->depth, (*it)->end, CV_RGB(255,0,0), 3, 8, 0);
							}
						}
					}else {
						if((*it)->depthAngle <=100) {
							fingerBlocks->push_back(block);

							IplImage* m_displayImage = ImageProcessor::Instance()->getDisplayedImage();
							cvCircle(m_displayImage, (*it)->start, 10, CV_RGB(255,0,0), 1, 8 ,0);
							cvCircle(m_displayImage, (*it)->end, 10, CV_RGB(0,255,0), 1, 8 ,0);
							cvCircle(m_displayImage, (*it)->depth, 10, CV_RGB(0,0,255), 1, 8 ,0);
							cvLine(m_displayImage, (*it)->start, (*it)->end, CV_RGB(0,0,255), 3, 8, 0);
							cvLine(m_displayImage, (*it)->start, (*it)->depth, CV_RGB(0,255,0), 3, 8, 0);
							cvLine(m_displayImage, (*it)->depth, (*it)->end, CV_RGB(255,0,0), 3, 8, 0);
						}
					}
					lastOrientation = (*it)->orientation;
				}
			}
		}
	}

	CvPoint pos = cvPoint(0, 0);
	int i = 0;


	for (std::list<DefectBlock*>::iterator it = fingerBlocks->begin(); it != fingerBlocks->end(); ++ it) {

		pos.x += (*it)->depth.x;
		pos.y += (*it)->depth.y;
		i++;
	}

	if(i > 0) {
		pos.x /= i;
		pos.y /= i;
	}


	delete fingerBlocks;
	m_Position = pos;

	return m_Position;
}

int Handjet::CandidateObject::getWidth() {
	return m_rect.width;
}

int Handjet::CandidateObject::getHeight() {
	return m_rect.height;
}

CvPoint Handjet::CandidateObject::getOrigin() {
	return cvPoint(m_rect.x, m_rect.y);
}

int Handjet::CandidateObject::getToken() {
	return token;
}

void Handjet::CandidateObject::setToken(int t) {
	this->token = t;
}

void Handjet::CandidateObject::setShape(int s) {
	this->m_currentShape = s;
}

int Handjet::CandidateObject::getShape() {
	return m_currentShape;
}

void Handjet::CandidateObject::setContoures(CvSeq* c) {

	this->m_contoures = c;

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* ptseq = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage );
	
	for(int i = 0; i < m_contoures->total; i++ ) {
		CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, m_contoures, i );
		cvSeqPush( ptseq, p );
	}

	CvSeq* hull = cvConvexHull2( ptseq, 0, CV_CLOCKWISE, 0 );
	CvRect rect = cvBoundingRect( ptseq, 1 );

	this->m_rect = rect;

	//cvRectangle( m_displayImage, cvPoint(rect.x, rect.y + rect.height), cvPoint(rect.x + rect.width, rect.y), CV_RGB(255, 255, 0), 1, 8, 0 );

	CvSeq* defect = cvConvexityDefects( ptseq, hull, storage );

	this->m_defects = defect;
	processDefects(m_defects);

	cvReleaseMemStorage( &storage );

	calcSignature();
}

void Handjet::CandidateObject::processDefects(CvSeq* firstDefect) {
	
	for(int counter = 0; firstDefect; firstDefect = firstDefect->h_next) {
		int nomdef = firstDefect->total;
	
		if(nomdef == 0)
			continue;
	
		CvConvexityDefect* defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*nomdef);
		cvCvtSeqToArray (firstDefect, defectArray, CV_WHOLE_SEQ);

		DefectBlock** blocks = (DefectBlock**)calloc(nomdef, sizeof(DefectBlock*));
	
		for(int i = 0; i<nomdef; i++) {
			blocks[i] = generateDefectBlock(&defectArray[i]);
			
			if(blocks[i]->area >=25) {
				blocks[i]->block_id = i;
				m_DefectBlocks->push_back(blocks[i]);
			}
		}

		int s = m_DefectBlocks->size();

		for(int i = s/2; i < s; i++) {
			m_DefectBlocks->push_front(m_DefectBlocks->back());
			m_DefectBlocks->pop_back();
		}

		free(defectArray);
	}
}

DefectBlock* Handjet::CandidateObject::generateDefectBlock(CvConvexityDefect* defectblock) {

	DefectBlock* block = (DefectBlock*)malloc(sizeof(DefectBlock));

	block->start = *defectblock->start;
	block->end = *defectblock->end;
	block->depth = *defectblock->depth_point;

	block->start_end_len = Handjet::ShapeUtil::calcLength(block->start, block->end);
	block->start_depth_len = Handjet::ShapeUtil::calcLength(block->start, block->depth);
	block->depth_end_len = Handjet::ShapeUtil::calcLength(block->depth, block->end);

	block->area = abs(cvTriangleArea(cvPointTo32f(block->start), cvPointTo32f(block->end), cvPointTo32f(block->depth)));
	block->depthAngle = Handjet::ShapeUtil::calcAngle(block->start_depth_len, block->depth_end_len, block->start_end_len);
	block->neighbors = 0;

	block->block_id = -1;
	block->depthLength = defectblock->depth;
	block->orientation = Handjet::ShapeUtil::calcOrientation(block->depthAngle, block->end, block->depth);

	return block;
}

void Handjet::CandidateObject::drawInVideo() {

	IplImage* m_displayImage = ImageProcessor::Instance()->getDisplayedImage();

	/*for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it)
	{
	cvCircle(m_displayImage, (*it)->start, 10, CV_RGB(255,0,0), 1, 8 ,0);
	cvCircle(m_displayImage, (*it)->end, 10, CV_RGB(0,255,0), 1, 8 ,0);
	cvCircle(m_displayImage, (*it)->depth, 10, CV_RGB(0,0,255), 1, 8 ,0);
	cvLine(m_displayImage, (*it)->start, (*it)->end, CV_RGB(0,0,255), 3, 8, 0);
	cvLine(m_displayImage, (*it)->start, (*it)->depth, CV_RGB(0,255,0), 3, 8, 0);
	cvLine(m_displayImage, (*it)->depth, (*it)->end, CV_RGB(255,0,0), 3, 8, 0);
	}*/
	double Scale=1;
	int Thickness=1;
	CvFont Font1=cvFont(Scale,Thickness);

	CvPoint  TextPosition1=cvPoint(50, 150);
	CvScalar  Color=CV_RGB(255,255,255);

	char p[100] = {0};
	//itoa((int)abs(blocks[i]->start_end_len *100/ ((blocks[i]->start_depth_len + blocks[i]->depth_end_len) / 2)), p, 10);
	itoa(judge(), p, 10);
	cvPutText(m_displayImage, p,TextPosition1,&Font1,Color);
}



void Handjet::CandidateObject::calcSignature() {

	m_signature.rectArea = m_rect.width * m_rect.height;
	m_signature.outerObjectArea = cvContourArea(m_contoures, CV_WHOLE_SEQ );
	m_signature.totalDefects = m_DefectBlocks->size();

	m_signature.defectArea = 0;
	m_signature.defectOrientation = 0;
	m_signature.defectsInThreshold = 0;
	
	for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it)
	{
		m_signature.defectArea += (*it)->area;
		//m_signature.defectOrientation += (*it)->orientation;
		if((*it)->area >= (0.05 * m_signature.outerObjectArea)) {
			m_signature.defectsInThreshold++;
			m_signature.defectOrientation += (*it)->orientation;
		}
	}

}

CandidateSignature Handjet::CandidateObject::getSignature() {
	return m_signature;
}

bool Handjet::CandidateObject::recheckIfFist() {

	int fingersExceptBig = 0;
	int semiCount = 0, openCount = 0;

	std::list<DefectBlock*>* fingerBlocks = new std::list<DefectBlock*>();

	for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it)
	{
		DefectBlock* block = *it;
		if(block->depthAngle <= 90) {
			//可能是手指之间的区块
			if(block->area > m_signature.defectArea * 0.05 && m_signature.defectArea > m_signature.rectArea * 0.2) {
				//面积足够大
				if(block->start_end_len < (block->depth_end_len + block->start_depth_len) / 3) {
					//手指伸直的可能性较大
					fingersExceptBig++;
					block->judgedValue = block->judgedValue | MAYBE_OPEN_PALM;
					openCount++;
				}
				else if(block->start_end_len > (block->depth_end_len + block->start_depth_len) / 3 && 
					block->start_end_len < (block->depth_end_len + block->start_depth_len) * 2 / 3) {
						//手指半伸的可能性较大
						fingersExceptBig++;
						block->judgedValue = block->judgedValue | MAYBE_SEMI_OPEN_PALM | MAYBE_SEMI_CLOSED_PALM;
						semiCount ++;
				}
			}
		}
	}

	int maybeFinferCount = openCount + semiCount;

	if(maybeFinferCount >=1) {
		if(openCount >= 3) {
			this->setShape(m_currentShape | MAYBE_FOUR);
		}
		return false;
	}

	return true;
}
//return the shape bitmap int value of the candidate
int Handjet::CandidateObject::judge() {

	int lastShape = getShape();
	if(lastShape != 0) {
		return lastShape;
	}
	setShape(0);

	int result= 0;
	int fingersExceptBig = 0;
	int bigToe = 0;
	
	int semiCount = 0, openCount = 0;

	std::list<DefectBlock*>* fingerBlocks = new std::list<DefectBlock*>();

	for (std::list<DefectBlock*>::iterator it = m_DefectBlocks->begin(); it != m_DefectBlocks->end(); ++ it)
	{
		DefectBlock* block = *it;
		if(block->depthAngle <= 100) {
			//可能是手指之间的区块
			if(block->area > m_signature.defectArea * 0.05 && m_signature.defectArea > m_signature.rectArea * 0.2) {
				//面积足够大
				if(block->start_end_len < (block->depth_end_len + block->start_depth_len) / 3) {
					//手指伸直的可能性较大
					fingersExceptBig++;
					block->judgedValue = block->judgedValue | MAYBE_OPEN_PALM;
					openCount++;
					fingerBlocks->push_back(block);
				}
				else if(block->start_end_len > (block->depth_end_len + block->start_depth_len) / 3 && 
					block->start_end_len < (block->depth_end_len + block->start_depth_len) * 5 / 6) {
					//手指半伸的可能性较大
					fingersExceptBig++;
					block->judgedValue = block->judgedValue | MAYBE_SEMI_OPEN_PALM | MAYBE_SEMI_CLOSED_PALM;
					semiCount ++;
					fingerBlocks->push_back(block);
				}
				
			}
		}
	}

	int neightborCount = 0;
	double lastOrientation = 0.0;

	int maybeFinferCount = 0;

	for (std::list<DefectBlock*>::iterator it = fingerBlocks->begin(); it != fingerBlocks->end(); ++ it) {

		if(lastOrientation > 0.01 || lastOrientation < -0.01) {
			if(abs((*it)->orientation - lastOrientation) < 60) {
				//result = MAYBE_HEAD | MAYBE_HEAD_WITH_NECK;
				maybeFinferCount++;
			}
		}
		lastOrientation = (*it)->orientation;
	
		if(findNeightborBlock((*it), fingerBlocks) > 0) {
			neightborCount++;
		}
	}

	delete fingerBlocks;

	if(fingersExceptBig > 0) {
		//手指数等于区块总数加一
		fingersExceptBig++;
	}

	if(fingersExceptBig >= 2 || neightborCount >=3) {
		result = result | MAYBE_NOT_FIST;
	}
	
	if(fingersExceptBig >= 3 && maybeFinferCount >= 3 && neightborCount >= 3) {
		//if(neightborCount >= 3) {
			result = MAYBE_LEFT_HAND | MAYBE_RIGHT_HAND;
		//}
		if(openCount >= semiCount) {
			result = result | MAYBE_OPEN_PALM | MAYBE_LEFT_HAND | MAYBE_RIGHT_HAND;
		}
		else {
			result = result | MAYBE_SEMI_OPEN_PALM | MAYBE_LEFT_HAND | MAYBE_RIGHT_HAND;
		}
	}
	//else if(!haveJudgeResultBit(result, MAYBE_HEAD)) {
	//	result = result | MAYBE_FIST;
	//}
	else {
		result = result | MAYBE_HEAD | MAYBE_HEAD_WITH_NECK;
	}

	setShape(result);

	return result;
}

bool Handjet::CandidateObject::haveJudgeResultBit(int result, int shape) {
	if(result & shape) {
		return true;
	}
	return false;
}

int Handjet::CandidateObject::findNeightborBlock(DefectBlock* block, std::list<DefectBlock*>* all) {
	int neightborCount = 0;
	for (std::list<DefectBlock*>::iterator it = all->begin(); it != all->end(); ++ it) {
		if(ShapeUtil::calcLength((*it)->start, block->end) < NEIGHBOR_DISTANCE ||
			ShapeUtil::calcLength((*it)->end, block->start) < NEIGHBOR_DISTANCE) {
			neightborCount++;
		}
	}

	return neightborCount;
}
