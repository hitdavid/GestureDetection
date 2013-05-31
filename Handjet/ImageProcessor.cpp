#include "stdafx.h"
#include "CvHeader.h"
#include "ImageProcessor.h"

using namespace std;
using namespace Handjet;

static Handjet::ImageProcessor* m_instance = NULL;

ImageProcessor* Handjet::ImageProcessor::Instance() {
	if(m_instance == NULL) {
		m_instance = new ImageProcessor();
	}
	return m_instance;
}

Handjet::ImageProcessor::ImageProcessor() {
	shouldStop = false;
	shouldShowVideo = false;

	m_runMode = 0;

	m_tracer = new CandidateTracer();
	m_candidates = new std::list<Handjet::CandidateObject*>();
	ifBlueMethodUsed = false;
}

Handjet::ImageProcessor::~ImageProcessor() {

	delete m_tracer;
	CandidateObject* co = NULL;
	while ( m_candidates->size() > 0)
	{
		co = m_candidates->back();
		//hc->clearAllDefectBlocks();
		delete co;
		co = NULL;
		m_candidates->pop_back();
	}
	delete m_candidates;

	m_displayImage = NULL;
}

void Handjet::ImageProcessor::process() {
	
	CvCapture* capture = cvCaptureFromCAM(0);

	IplImage* frame  = cvCreateImage( cvSize(640,480), IPL_DEPTH_8U, 3 );

	while(!shouldStop)
	{
		
		m_candidates->clear();
		frame = cvQueryFrame(capture);
		processFrame(frame);
		m_tracer->updateCandidates(m_candidates);
		
		if(shouldShowVideo && m_displayImage) {
			cvShowImage( "result", m_displayImage );
		}
		waitKey(10);
	}
	

	//cvReleaseImage(&frame);
	frame = NULL;
	cvReleaseCapture( &capture );
}


void Handjet::ImageProcessor::processFrame(IplImage* frame) {

	IplImage* img_bw1 = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
	IplImage* img_bw = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
	cvFlip(frame, frame,4);

	if(ifBlueMethodUsed) {
		SkinRGB(frame, img_bw1);
		cvSkinOtsu(img_bw1, img_bw);
	}
	else {
		//SkinRGB(frame, img_bw1);
		cvSkinOtsu(frame, img_bw);
	}
	
	//cvCvtColor(img_bw1, img_bw, 6);
	//cvSkinYUV(frame, img_bw);

	cvThreshold(img_bw, img_bw, 30, 255, CV_THRESH_BINARY);
	cvErode(img_bw,img_bw, 0, 3);
	cvSmooth(img_bw,img_bw, 2, 3, 3, 0, 0);
	cvDilate(img_bw,img_bw, 0, 1);

	m_displayImage = frame;
	scanCandidate(img_bw, frame);
	cvReleaseImage(&img_bw1);
	cvReleaseImage(&img_bw);
	img_bw = NULL;
	img_bw1 = NULL;
}

void Handjet::ImageProcessor::stop() {
	shouldStop = true;
}

void Handjet::ImageProcessor::showVideo() {
	cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
	shouldShowVideo = true;
}

void Handjet::ImageProcessor::stopVideo() {
	shouldShowVideo = false;
	cvDestroyWindow("result");
}

void Handjet::ImageProcessor::blueMethodSwitch() {
	ifBlueMethodUsed = !ifBlueMethodUsed;
}

void Handjet::ImageProcessor::showTracing() {
	
}


int Handjet::ImageProcessor::setRunMode(int mode) {
	m_runMode = mode;
	return m_runMode;
}

int Handjet::ImageProcessor::getRunMode() {
	return m_runMode;
}

void Handjet::ImageProcessor::SkinRGB(IplImage* rgb,IplImage* _dst)
{
    assert(rgb->nChannels==3&& _dst->nChannels==3);

    static const int R=2;
    static const int G=1;
    static const int B=0;

    IplImage* dst=cvCreateImage(cvGetSize(_dst),8,3);
    cvZero(dst);

    for (int h=0;h<rgb->height;h++) {
        unsigned char* prgb=(unsigned char*)rgb->imageData+h*rgb->widthStep;
        unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
        for (int w=0;w<rgb->width;w++) {
            if ((prgb[R]>95 && prgb[G]>40 && prgb[B]>20 &&
                 prgb[R]-prgb[B]>15 && prgb[R]-prgb[G]>15 &&
                     !(prgb[R]>170&&prgb[G]>170&&prgb[B]>170))||//uniform illumination
                    (prgb[R]>200 && prgb[G]>210 && prgb[B]>170 &&
                     abs(prgb[R]-prgb[B])<=15 && prgb[R]>prgb[B]&& prgb[G]>prgb[B])//lateral illumination
                    ) {
                memcpy(pdst,prgb,3);
            }
            prgb+=3;
            pdst+=3;
        }
    }
    cvCopyImage(dst,_dst);
    cvReleaseImage(&dst);
}

void Handjet::ImageProcessor::cvSkinRG(IplImage* rgb,IplImage* gray)
{
    assert(rgb->nChannels==3&&gray->nChannels==1);

    const int R=2;
    const int G=1;
    const int B=0;

    double Aup=-1.8423;
    double Bup=1.5294;
    double Cup=0.0422;
    double Adown=-0.7279;
    double Bdown=0.6066;
    double Cdown=0.1766;
    for (int h=0; h<rgb->height; h++)
    {
        unsigned char* pGray=(unsigned char*)gray->imageData+h*gray->widthStep;
        unsigned char* pRGB=(unsigned char* )rgb->imageData+h*rgb->widthStep;
        for (int w=0; w<rgb->width; w++)
        {
            int s=pRGB[R]+pRGB[G]+pRGB[B];
            double r=(double)pRGB[R]/s;
            double g=(double)pRGB[G]/s;
            double Gup=Aup*r*r+Bup*r+Cup;
            double Gdown=Adown*r*r+Bdown*r+Cdown;
            double Wr=(r-0.33)*(r-0.33)+(g-0.33)*(g-0.33);
            if (g<Gup && g>Gdown && Wr>0.004)
            {
                *pGray=255;
            }
            else
            {
                *pGray=0;
            }
            pGray++;
            pRGB+=3;
        }
    }

}

void Handjet::ImageProcessor::cvThresholdOtsu(IplImage* src, IplImage* dst)
{
    int height=src->height;
    int width=src->width;

    //histogram
    float histogram[256]= {0};
    for(int i=0; i<height; i++)
    {
        unsigned char* p=(unsigned char*)src->imageData+src->widthStep*i;
        for(int j=0; j<width; j++)
        {
            histogram[*p++]++;
        }
    }
    //normalize histogram
    int size=height*width;
    for(int i=0; i<256; i++)
    {
        histogram[i]=histogram[i]/size;
    }

    //average pixel value
    float avgValue=0;
    for(int i=0; i<256; i++)
    {
        avgValue+=i*histogram[i];
    }

    int threshold;
    float maxVariance=0;
    float w=0,u=0;
    for(int i=0; i<256; i++)
    {
        w+=histogram[i];
        u+=i*histogram[i];

        float t=avgValue*w-u;
        float variance=t*t/(w*(1-w));
        if(variance>maxVariance)
        {
            maxVariance=variance;
            threshold=i;
        }
    }
    cvThreshold(src,dst,threshold,255,CV_THRESH_BINARY);
}

void Handjet::ImageProcessor::cvSkinOtsu(IplImage* src, IplImage* dst)
{
    assert(dst->nChannels==1&& src->nChannels==3);

    IplImage* ycrcb=cvCreateImage(cvGetSize(src),8,3);
    IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
    cvCvtColor(src,ycrcb,CV_BGR2YCrCb);
    cvSplit(ycrcb,0,cr,0,0);

    cvThresholdOtsu(cr,cr);
    cvCopyImage(cr,dst);
    cvReleaseImage(&cr);
    cvReleaseImage(&ycrcb);
}

void Handjet::ImageProcessor::cvSkinYCbCr(IplImage* img, IplImage* mask)
{
    CvSize imageSize = cvSize(img->width, img->height);
    IplImage *imgY = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);
    IplImage *imgCr = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);
    IplImage *imgCb = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);


    IplImage *imgYCrCb = cvCreateImage(imageSize, img->depth, img->nChannels);
    cvCvtColor(img,imgYCrCb,CV_BGR2YCrCb);
    cvSplit(imgYCrCb, imgY, imgCr, imgCb, 0);
    int y, cr, cb, l, x1, y1, value;
    unsigned char *pY, *pCr, *pCb, *pMask;

    pY = (unsigned char *)imgY->imageData;
    pCr = (unsigned char *)imgCr->imageData;
    pCb = (unsigned char *)imgCb->imageData;
    pMask = (unsigned char *)mask->imageData;
    cvSetZero(mask);
    l = img->height * img->width;
    for (int i = 0; i < l; i++){
        y  = *pY;
        cr = *pCr;
        cb = *pCb;
        cb -= 109;
        cr -= 152
                ;
        x1 = (819*cr-614*cb)/32 + 51;
        y1 = (819*cr+614*cb)/32 + 77;
        x1 = x1*41/1024;
        y1 = y1*73/1024;
        value = x1*x1+y1*y1;
        if(y<100)	(*pMask)=(value<700) ? 255:0;
        else		(*pMask)=(value<850)? 255:0;
        pY++;
        pCr++;
        pCb++;
        pMask++;
    }
    cvReleaseImage(&imgY);
    cvReleaseImage(&imgCr);
    cvReleaseImage(&imgCb);
    cvReleaseImage(&imgYCrCb);
}

void Handjet::ImageProcessor::cvSkinYUV(IplImage* src,IplImage* dst)
{
    IplImage* ycrcb=cvCreateImage(cvGetSize(src),8,3);
    //IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
    //IplImage* cb=cvCreateImage(cvGetSize(src),8,1);
    cvCvtColor(src,ycrcb,CV_BGR2YCrCb);
    //cvSplit(ycrcb,0,cr,cb,0);

    static const int Cb=2;
    static const int Cr=1;
    static const int Y=0;

    //IplImage* dst=cvCreateImage(cvGetSize(_dst),8,3);
    cvZero(dst);

    for (int h=0; h<src->height; h++)
    {
        unsigned char* pycrcb=(unsigned char*)ycrcb->imageData+h*ycrcb->widthStep;
        unsigned char* psrc=(unsigned char*)src->imageData+h*src->widthStep;
        unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
        for (int w=0; w<src->width; w++)
        {
            if (pycrcb[Cr]>=133&&pycrcb[Cr]<=173&&pycrcb[Cb]>=77&&pycrcb[Cb]<=127)
            {
                memcpy(pdst,psrc,3);
            }
            pycrcb+=3;
            psrc+=3;
            pdst+=3;
        }
    }
    //cvCopyImage(dst,_dst);
    //cvReleaseImage(&dst);
}

void Handjet::ImageProcessor::cvSkinHSV(IplImage* src,IplImage* dst)
{
    IplImage* hsv=cvCreateImage(cvGetSize(src),8,3);
    //IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
    //IplImage* cb=cvCreateImage(cvGetSize(src),8,1);
    cvCvtColor(src,hsv,CV_BGR2HSV);
    //cvSplit(ycrcb,0,cr,cb,0);

    static const int V=2;
    static const int S=1;
    static const int H=0;

    //IplImage* dst=cvCreateImage(cvGetSize(_dst),8,3);
    cvZero(dst);

    for (int h=0; h<src->height; h++)
    {
        unsigned char* phsv=(unsigned char*)hsv->imageData+h*hsv->widthStep;
        unsigned char* psrc=(unsigned char*)src->imageData+h*src->widthStep;
        unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
        for (int w=0; w<src->width; w++)
        {
            if (phsv[H]>=7&&phsv[H]<=29)
            {
                memcpy(pdst,psrc,3);
            }
            phsv+=3;
            psrc+=3;
            pdst+=3;
        }
    }
    //cvCopyImage(dst,_dst);
    //cvReleaseImage(&dst);
}


void Handjet::ImageProcessor::scanCandidate(IplImage* motion, IplImage* display)
{
	assert(motion != NULL && display != NULL);

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;

	double area=0,areamax=0;
	int maxn=0, n=0;

	int Nc = cvFindContours(motion, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));

	cvDrawContours( display, first_contour,  CV_RGB( 0, 200, 0), CV_RGB( 0, 100, 0), 1, 2, 8, cvPoint(0,0));

	if(Nc>0)
	{
		CvSeq* c;
		int i = 0;
		for( c=first_contour; c!=NULL; c=c->h_next, i++ )
		{
			area=cvContourArea(c, CV_WHOLE_SEQ );
			if(area > 5000) {
				CandidateObject* candidate = new CandidateObject();
				candidate->setContoures(c);
				candidate->setToken(i);
				m_candidates->push_back(candidate);
			}
		}
	}

	cvReleaseMemStorage( &storage );
}

IplImage* Handjet::ImageProcessor::getDisplayedImage() {
	return m_displayImage;
}



//void Handjet::ImageProcessor::SkinRGB(IplImage* rgb,IplImage* _dst)
//{
//	assert(rgb->nChannels==3&& _dst->nChannels==3);
//
//	static const int R=2;
//	static const int G=1;
//	static const int B=0;
//
//	IplImage* dst=cvCreateImage(cvGetSize(_dst),8,3);
//	cvZero(dst);
//
//	for (int h=0;h<rgb->height;h++) {
//		unsigned char* prgb=(unsigned char*)rgb->imageData+h*rgb->widthStep;
//		unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
//		for (int w=0;w<rgb->width;w++) {
//			if (
//				(prgb[R] > 65 && prgb[R] < 150 && prgb[G]>40 && prgb[B]>20 &&
//				prgb[R]-prgb[B]>15 && prgb[R]-prgb[G]>15
//				&&!(prgb[R]>170&&prgb[G]>170&&prgb[B]>170)
//				||//uniform illumination
//				(prgb[R]>200 && prgb[G]>210 && prgb[B]>170 &&
//				abs(prgb[R]-prgb[B])<=15 && prgb[R]>prgb[B]&& prgb[G]>prgb[B])//lateral illumination
//				) ){
//					memcpy(pdst,prgb,3);
//			}
//			prgb+=3;
//			pdst+=3;
//		}
//	}
//	cvCopyImage(dst,_dst);
//	cvReleaseImage(&dst);
//}
//
//void Handjet::ImageProcessor::cvThresholdOtsu(IplImage* src, IplImage* dst)
//{
//	int height=src->height;
//	int width=src->width;
//
//	//histogram
//	float histogram[256]={0};
//	for(int i=0;i<height;i++) {
//		unsigned char* p=(unsigned char*)src->imageData+src->widthStep*i;
//		for(int j=0;j<width;j++) {
//			histogram[*p++]++;
//		}
//	}
//	//normalize histogram
//	int size=height*width;
//	for(int i=0;i<256;i++) {
//		histogram[i]=histogram[i]/size;
//	}
//
//	//average pixel value
//	float avgValue=0;
//	for(int i=0;i<256;i++) {
//		avgValue+=i*histogram[i];
//	}
//
//	int threshold;
//	float maxVariance=0;
//	float w=0,u=0;
//	for(int i=0;i<256;i++) {
//		w+=histogram[i];
//		u+=i*histogram[i];
//
//		float t=avgValue*w-u;
//		float variance=t*t/(w*(1-w));
//		if(variance>maxVariance) {
//			maxVariance=variance;
//			threshold=i;
//		}
//	}
//
//	cvThreshold(src,dst,threshold,255,CV_THRESH_BINARY);
//}
//
//void Handjet::ImageProcessor::cvSkinOtsu(IplImage* src, IplImage* dst)
//{
//	assert(dst->nChannels==1&& src->nChannels==3);
//
//	IplImage* ycrcb=cvCreateImage(cvGetSize(src),8,3);
//	IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
//	cvCvtColor(src,ycrcb,CV_BGR2YCrCb);
//	cvSplit(ycrcb,0,cr,0,0);
//
//	cvThresholdOtsu(cr,cr);
//	cvCopyImage(cr,dst);
//	cvReleaseImage(&cr);
//	cvReleaseImage(&ycrcb);
//}
//
//