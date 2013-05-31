#pragma once

#include "CvHeader.h"
#include "CandidateTracer.h"
#include "DefectBlock.h"

using namespace cv;
using namespace std;

namespace Handjet {
	class ImageProcessor{

	private: 
		CandidateTracer* m_tracer;
		std::list<Handjet::CandidateObject*>* m_candidates;
		bool shouldStop;
		bool shouldShowVideo;
		IplImage* m_bwImage, *m_displayImage;
		int m_runMode;
		bool ifBlueMethodUsed;
	private:
		ImageProcessor();
		
		void processFrame(IplImage* frame);

		void SkinRGB(IplImage* rgb,IplImage* _dst);
		void cvSkinRG(IplImage* rgb,IplImage* gray);
		void cvThresholdOtsu(IplImage* src, IplImage* dst);
		void cvSkinOtsu(IplImage* src, IplImage* dst);
		void cvSkinYCbCr(IplImage* img, IplImage* mask);
		void cvSkinYUV(IplImage* src,IplImage* dst);
		void cvSkinHSV(IplImage* src,IplImage* dst);

		void scanCandidate(IplImage* img_8uc1, IplImage* img_8uc3);
		void processContours(CvSeq* contour);
		void processDefects(CvSeq* firstDefect);
		DefectBlock* generate_defect_block(CvConvexityDefect* defectblock);

		
	public: 
		static ImageProcessor* Instance();
		~ImageProcessor();
		void process();
		void stop();
		void showVideo();
		void stopVideo();
		void showTracing();

		IplImage* getDisplayedImage();
		int setRunMode(int mode);
		int getRunMode();

		void blueMethodSwitch();

	};
}