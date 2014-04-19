#include <stdio.h>
#include "facedet.h"

CvHaarClassifierCascade* GpDetector = 0;
CvMemStorage* GpStorage = 0;
CvSeq* GpFaceRectSeq;

int initFaceDet(const char* sHaarCascadeFilePath)
{
	GpStorage = cvCreateMemStorage(0);
	if(GpStorage == 0)
	{
		printf("ERROR: Can not allocate memory for face detection!\n");
		return 0;
	}

	GpDetector = (CvHaarClassifierCascade*) cvLoad(sHaarCascadeFilePath, 0, 0, 0);
	if(GpDetector == 0)
	{
		printf("ERROR: Can not load Haar classifier cascade from %s!\n", sHaarCascadeFilePath);
		return 0;
	}

	return 1;
}

void closeFaceDet()
{
	if(GpStorage != 0)
		cvReleaseMemStorage(&GpStorage);

	if(GpDetector != 0)
		cvReleaseHaarClassifierCascade(&GpDetector);
}

int detectFaces(IplImage* pInputImg, CvRect** pFaceRects, int iMaxFaceCnt)
{
	int iFaceNo = 0;
	int iDetectedFaceCnt = 0;
	
	CvSize minFaceSize = cvSize(0, 0);
	CvSize maxFaceSize = cvSize(pInputImg->width, pInputImg->height);

	GpFaceRectSeq = cvHaarDetectObjects(pInputImg, GpDetector, GpStorage, 1.1, 6, CV_HAAR_DO_CANNY_PRUNING, minFaceSize, maxFaceSize);
	if((GpFaceRectSeq != 0) && (GpFaceRectSeq->total != 0))
	{
		iDetectedFaceCnt = GpFaceRectSeq->total;
		if(iDetectedFaceCnt > iMaxFaceCnt)
			iDetectedFaceCnt = iMaxFaceCnt;
		
		for(iFaceNo = 0; iFaceNo < iDetectedFaceCnt; iFaceNo++)
			pFaceRects[iFaceNo] = (CvRect*)cvGetSeqElem(GpFaceRectSeq, iFaceNo);
	}

	return iDetectedFaceCnt;
}
