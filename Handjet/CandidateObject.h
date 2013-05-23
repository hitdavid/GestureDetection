#pragma once

#include "CvHeader.h"
#include "DefectBlock.h"
#include "CandidateSignature.h"

using namespace std;

namespace Handjet {
	class CandidateObject
	{
	public:
		const static unsigned int  MAYBE_HEAD = 1;
		const static unsigned int  MAYBE_HEAD_WITH_NECK = 2;
		const static unsigned int  MAYBE_LEFT_HAND = 4;
		const static unsigned int  MAYBE_RIGHT_HAND = 8;
		const static unsigned int  MAYBE_SEMI_OPEN_PALM = 16;
		const static unsigned int  MAYBE_SEMI_CLOSED_PALM = 32;
		const static unsigned int  MAYBE_FIST = 64;
		const static unsigned int  MAYBE_FINGER_TOGETHER_PALM = 128;
		const static unsigned int  MAYBE_RSHIFT_OPEN_PALM = 256;
		const static unsigned int  MAYBE_LSHIFT_OPEN_PALM = 512;
		const static unsigned int  MAYBE_OPEN_PALM = 1024;
		const static unsigned int  MAYBE_ONE = 8192;
		const static unsigned int  MAYBE_TWO = 16384;
		const static unsigned int  MAYBE_FOUR = 32768;
		const static unsigned int  MAYBE_SEVEN = 65536;

		const static unsigned int MAYBE_NOT_FIST = 2048;


	private:
		CvSeq* m_contoures;
		CvSeq* m_defects;
		CvRect m_rect;
		CvPoint m_center;
		int token;
		CandidateSignature m_signature;

		std::list<DefectBlock*>* m_DefectBlocks;

		int m_currentShape;
		CvPoint m_Position;
	private:
		void processDefects(CvSeq* firstDefect);
		DefectBlock* generateDefectBlock(CvConvexityDefect* defectblock);

		void calcSignature();
		int findNeightborBlock(DefectBlock* block, std::list<DefectBlock*>* all);

	public:
		CandidateObject(void);
		~CandidateObject(void);

		CvPoint getValidPosition();
		CvPoint getFistPosition();

		CvPoint getCenter();
		int getWidth();
		int getHeight();
		CvPoint getOrigin();

		void setShape(int s);
		int getShape();

		int getToken();
		void setToken(int token);

		CandidateSignature getSignature();

		void setContoures(CvSeq* c);

		void drawInVideo();
		int judge();
		bool recheckIfFist();
		static bool haveJudgeResultBit(int result, int shape);
	};
}

