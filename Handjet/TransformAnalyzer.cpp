#include "stdafx.h"
using namespace std;

#include "TransformAnalyzer.h"
#include "CandidateTracer.h"
#include "HandCandidate.h"
#include "CandidateObject.h"
#include "TracingTableItem.h"
#include "ShapeUtil.h"
#include "CandidateSignature.h"
#include "ImageProcessor.h"

Handjet::TransformAnalyzer::TransformAnalyzer() {
	m_tracingTable = new list<TracingTableItem*>();
}

Handjet::TransformAnalyzer::~TransformAnalyzer() {
	delete m_tracingTable;
}


CandidateObject* Handjet::TransformAnalyzer::analyze(list<CandidateObject*>* lastFrame, list<CandidateObject*>* thisFrame, list<TracingTableItem*>* tracingTable) {
	
	CandidateObject* result = NULL;
	int thisSize = thisFrame -> size();
	int lastSize = lastFrame -> size();

	bool clearFlagInThisFrame = false;

	if(thisSize == 0 && lastSize == 0) {
		return NULL;
	}

	for(list<CandidateObject*>::iterator it = thisFrame->begin(); it != thisFrame->end(); ++ it) {
		int shape = (*it)->judge();
		if(CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_OPEN_PALM) ||
			CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_SEMI_OPEN_PALM) ||
			CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_SEMI_CLOSED_PALM) ||
			CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_FIST)) {
				(*it)->drawInVideo();
				result = (*it);
				//clearFlagInThisFrame = true;
				return result;
		}
	}


	for(list<CandidateObject*>::iterator it = thisFrame->begin(); it != thisFrame->end(); ++ it) {
		
		CandidateObject* lastApperence = findObjectInLastFrame((*it), lastFrame);
		if(lastApperence == NULL) {
			continue;
		}

		//»æÖÆÖ¡¼äÒÆ¶¯
		IplImage* image = ImageProcessor::Instance()->getDisplayedImage();
		cvLine(image, (*it)->getCenter(), lastApperence->getCenter(), CV_RGB(255,0,0), 3, 8, 0);

		int shape = (*it)->judge();
		int lastShape = lastApperence->judge();
		
		


		if(CandidateObject::haveJudgeResultBit(lastShape, CandidateObject::MAYBE_LEFT_HAND) ||
			CandidateObject::haveJudgeResultBit(lastShape, CandidateObject::MAYBE_RIGHT_HAND)) {
				if(!CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_OPEN_PALM) &&
					!CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_SEMI_OPEN_PALM) &&
					!CandidateObject::haveJudgeResultBit(shape, CandidateObject::MAYBE_SEMI_CLOSED_PALM)) {
						(*it)->setShape(CandidateObject::MAYBE_FIST | CandidateObject::MAYBE_LEFT_HAND | CandidateObject::MAYBE_RIGHT_HAND);
						(*it)->drawInVideo();
						result = (*it);
						break;
				}
		}

		if(CandidateObject::haveJudgeResultBit(lastShape, CandidateObject::MAYBE_LEFT_HAND) ||
			CandidateObject::haveJudgeResultBit(lastShape, CandidateObject::MAYBE_RIGHT_HAND)) {
				(*it)->setShape(shape | CandidateObject::MAYBE_LEFT_HAND | CandidateObject::MAYBE_RIGHT_HAND);
				(*it)->drawInVideo();
				result = (*it);
				//break;
		}
	}

	return result;
}


TracingTableItem* Handjet::TransformAnalyzer::findTracingTableItem(int token) {
	if(m_tracingTable == NULL) {
		m_tracingTable = new list<TracingTableItem*>();
		return NULL;
	}
	if(m_tracingTable->size() == 0) {
		return NULL;
	}
	for(list<TracingTableItem*>::iterator it = m_tracingTable->begin(); it != m_tracingTable->end(); ++ it) {
		if((*it)->m_candidate->getToken() == token) {
			return *it;
		}
	}
	return NULL;
}

CandidateObject* Handjet::TransformAnalyzer::findObjectInLastFrame(CandidateObject* candidate, list<CandidateObject*>* lastFrame) {

	int *scores = (int*)malloc(lastFrame->size() * sizeof(int) * 2);
	int index = 0;
	CandidateObject* result = NULL;
	int len = lastFrame->size();
	int maxInt = MAXINT - 1;
	
	for(list<CandidateObject*>::iterator it = lastFrame->begin(); it != lastFrame->end(); ++ it) {
		// position scores
		int posScore = calcPositionScore((*it), candidate);
		scores[index] = posScore * posScore;
		// shape scores
		scores[index] += calcShapeScore((*it), candidate) / 100;

		if(scores[index] <= maxInt) {
			result = (*it);
			maxInt = scores[index];
		}
		index++;
	}
	return result;
}

int Handjet::TransformAnalyzer::calcPositionScore(CandidateObject* object, CandidateObject* candidate) {
	int score = 0;
	score = Handjet::ShapeUtil::calcLength(object->getCenter(), candidate->getCenter());
	score += Handjet::ShapeUtil::calcLength(object->getOrigin(), candidate->getOrigin());
	//score += abs(object->getHeight() - candidate->getHeight());
	//score += abs(object->getWidth() - candidate->getWidth());
	return score;
}

int Handjet::TransformAnalyzer::calcShapeScore(CandidateObject* object, CandidateObject* candidate) {
	int score = 0;
	CandidateSignature objectSignature =  object->getSignature();
	CandidateSignature candidateSignature =  candidate->getSignature();

	score += abs(objectSignature.outerObjectArea - candidateSignature.outerObjectArea);
	score += abs(objectSignature.totalDefects - candidateSignature.totalDefects);
	score += abs(objectSignature.defectArea - candidateSignature.defectArea);
	score += abs(objectSignature.defectOrientation - candidateSignature.defectOrientation);
	score += abs(objectSignature.defectsInThreshold - candidateSignature.defectsInThreshold);

	return score;
}


CandidateObject* Handjet::TransformAnalyzer::findHandCandidate(list<CandidateObject*>* candidates) {
	throw "Not yet implemented";
}

