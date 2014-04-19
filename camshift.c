#include "cv.h"
#include <stdio.h>
#include "camshift.h"

void updateHueImage(stCamShift* pTracker, const IplImage* pInputImg);

void createTracker(stCamShift* cs, const CvSize imgSize)
{
	float* pRanges = cs->fArrHistogramRanges;

	cs->iNumberOfHistBins = 30;
	cs->fArrHistogramRanges[0] = 0;
	cs->fArrHistogramRanges[1] = 180;

	cs->iVMin = 30;
	cs->iVMax = 75;
	cs->iSMin = 30;

	cs->iNumberOfFrames = 0;

	cs->pHSVImg = cvCreateImage(imgSize, 8, 3);
	cs->pHueImg = cvCreateImage(imgSize, 8, 1);
	cs->pMask = cvCreateImage(imgSize, 8, 1);
	cs->pFaceProbImg = cvCreateImage(imgSize, 8, 1);
	cs->pHist = cvCreateHist(1, &cs->iNumberOfHistBins, CV_HIST_ARRAY, &pRanges, 1);
}

void releaseTracker(stCamShift* pTracker)
{
	cvReleaseImage(&(pTracker->pHSVImg));
	cvReleaseImage(&(pTracker->pHueImg));
	cvReleaseImage(&(pTracker->pMask) );
	cvReleaseImage(&(pTracker->pFaceProbImg));

	cvReleaseHist(&(pTracker->pHist));
}

void startTracking(stCamShift* pTracker, IplImage* pInputImg, CvRect* pFaceRect)
{
	float maxVal = 0.f;

	updateHueImage(pTracker, pInputImg);

	cvSetImageROI(pTracker->pHueImg, *pFaceRect);
	cvSetImageROI(pTracker->pMask, *pFaceRect);
	cvCalcHist(&pTracker->pHueImg, pTracker->pHist, 0, pTracker->pMask);
	cvGetMinMaxHistValue(pTracker->pHist, 0, &maxVal, 0, 0);
	cvConvertScale(pTracker->pHist->bins, pTracker->pHist->bins, maxVal? 255.0/maxVal : 0, 0);
	cvResetImageROI(pTracker->pHueImg);
	cvResetImageROI(pTracker->pMask);

	pTracker->prevFaceRect = *pFaceRect;
}

CvBox2D track(stCamShift* pTracker, IplImage* pInputImg)
{
	CvConnectedComp components;

	updateHueImage(pTracker, pInputImg);
	cvCalcBackProject(&pTracker->pHueImg, pTracker->pFaceProbImg, pTracker->pHist);
    cvAnd(pTracker->pFaceProbImg, pTracker->pMask, pTracker->pFaceProbImg, 0);

    cvCamShift(pTracker->pFaceProbImg, pTracker->prevFaceRect, cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1), &components, &pTracker->faceBox);

    pTracker->prevFaceRect = components.rect;
	/*cs->faceBox.angle = -cs->faceBox.angle;*/

	return pTracker->faceBox;
}

void updateHueImage(stCamShift* pTracker, const IplImage* pInputImg)
{
	cvCvtColor(pInputImg, pTracker->pHSVImg, CV_BGR2HSV);

	cvInRangeS(pTracker->pHSVImg, cvScalar(0, pTracker->iSMin, MIN(pTracker->iVMin, pTracker->iVMax), 0),
				cvScalar(180, 256, MAX(pTracker->iVMin, pTracker->iVMax), 0), pTracker->pMask);

	cvSplit(pTracker->pHSVImg, pTracker->pHueImg, 0, 0, 0);
}
