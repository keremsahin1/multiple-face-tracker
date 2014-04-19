#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include "capture.h"
#include "facedet.h"
#include "camshift.h"
#include <sys/time.h>

#define FACE_DETECTION_XML_PATH  "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_default.xml"
#define NUMBER_OF_MAX_FACES 2

const char* DISPLAY_WINDOW = "DisplayWindow";

stCamShift GarrTrackers[NUMBER_OF_MAX_FACES];
CvScalar GarrFaceBoxColors[NUMBER_OF_MAX_FACES];
IplImage *GpCurrentVideoFrame = 0, *GpPrevVideoFrame = 0;

int initAll();
void exitProgram(int iExitCode);
void updateScreen();

int main(int iArgCnt, char** sArrArgs)
{
	CvRect* arrFaceRects[NUMBER_OF_MAX_FACES];
	CvBox2D faceBox[NUMBER_OF_MAX_FACES];
	int iFaceNo = 0, iDetectedFaceCnt = 0;

	if(initAll() == 0)
	{
		exitProgram(-1);
	}

	while(1)
	{
		GpCurrentVideoFrame = nextVideoFrame();
		iDetectedFaceCnt = detectFaces(GpCurrentVideoFrame, arrFaceRects, NUMBER_OF_MAX_FACES);
		updateScreen();

		if(iDetectedFaceCnt > 0)
			break;
	}

	printf("INFO: Detected Face Count:%d\n", iDetectedFaceCnt);

	for(iFaceNo = 0; iFaceNo < iDetectedFaceCnt; iFaceNo++)
	{
		startTracking(&GarrTrackers[iFaceNo], GpCurrentVideoFrame, arrFaceRects[iFaceNo]);
	}

	while(1)
	{
		GpCurrentVideoFrame = nextVideoFrame();

		for(iFaceNo = 0; iFaceNo < iDetectedFaceCnt; iFaceNo++)
		{
			faceBox[iFaceNo] = track(&GarrTrackers[iFaceNo], GpCurrentVideoFrame);
		}

		for(iFaceNo = 0; iFaceNo < iDetectedFaceCnt; iFaceNo++)
		{
			cvEllipseBox(GpCurrentVideoFrame, faceBox[iFaceNo], GarrFaceBoxColors[iFaceNo], 3, CV_AA, 0);
		}

		updateScreen();
	}

	exitProgram(0);
	return 0;
}

void updateScreen()
{
	struct timeval tm1;

	gettimeofday(&tm1, NULL);
	cvShowImage(DISPLAY_WINDOW, GpCurrentVideoFrame);
	cvReleaseImage(&GpPrevVideoFrame);
	cvWaitKey(1);

	printf("INFO: Screen updated @%d.%06d\n", (int)(tm1.tv_sec % 60), (int)tm1.tv_usec);

	GpPrevVideoFrame = GpCurrentVideoFrame;
}

int initAll()
{
	int iTrackerNo = 0;
	CvSize imgSize;

	if(initCapture() == 0)
		return 0;
	if(initFaceDet(FACE_DETECTION_XML_PATH) == 0)
		return 0;

	imgSize = cvGetSize(nextVideoFrame());
	for(iTrackerNo = 0; iTrackerNo < NUMBER_OF_MAX_FACES; iTrackerNo++)
	{
		createTracker(&GarrTrackers[iTrackerNo], imgSize);
	}

	for(iTrackerNo = 0; iTrackerNo < NUMBER_OF_MAX_FACES; iTrackerNo++)
	{
		GarrFaceBoxColors[iTrackerNo] = CV_RGB(iTrackerNo * (255/NUMBER_OF_MAX_FACES), 0, 0);
	}

	cvNamedWindow(DISPLAY_WINDOW, 1);

	return 1;
}

void exitProgram(int iExitCode)
{
	int iTrackerNo = 0;

	cvDestroyWindow(DISPLAY_WINDOW);
	cvReleaseImage(&GpCurrentVideoFrame);

	closeCapture();
	closeFaceDet();

	for (iTrackerNo = 0; iTrackerNo < NUMBER_OF_MAX_FACES; iTrackerNo++)
	{
		releaseTracker(&GarrTrackers[iTrackerNo]);
	}

	exit(iExitCode);
}

