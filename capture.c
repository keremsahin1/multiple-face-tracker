#include <stdio.h>
#include "highgui.h"
#include "capture.h"

CvCapture* GpCapture = 0;

int initCapture()
{
	GpCapture = cvCaptureFromCAM(CV_CAP_ANY);
	if(GpCapture == 0)
	{
		printf("ERROR: Failed to initialize video capture!\n");
		return 0;
	}

	return 1;
}

void closeCapture()
{
	cvReleaseCapture(&GpCapture);
	return;
}

IplImage* nextVideoFrame()
{
	IplImage* pVideoFrame = cvQueryFrame(GpCapture);
	IplImage* pVideoFrameProcessed = 0;

	if(pVideoFrame == 0)
		printf("ERROR: Failed to get a video frame!\n");
	else
	{
		pVideoFrameProcessed = cvCreateImage(cvGetSize(pVideoFrame), 8, 3);
		cvCopy(pVideoFrame, pVideoFrameProcessed, 0);
		pVideoFrameProcessed->origin = pVideoFrame->origin;

		if(pVideoFrameProcessed->origin == 0)
		{
			cvFlip(pVideoFrameProcessed, 0, 0);
			pVideoFrameProcessed->origin = 1;
		}
	}

	return pVideoFrameProcessed;
}

