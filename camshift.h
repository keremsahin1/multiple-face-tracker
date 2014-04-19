#ifndef CAMSHIFT_H
#define CAMSHIFT_H

typedef struct camShift
{
  int iNumberOfHistBins;
  float fArrHistogramRanges[2];
  int iVMin; 
  int iVMax;
  int iSMin;

  IplImage* pHSVImg;
  IplImage* pHueImg;
  IplImage* pMask;
  IplImage* pFaceProbImg;
  CvHistogram* pHist;
  
  CvRect prevFaceRect;
  CvBox2D faceBox;
  int iNumberOfFrames;
} stCamShift;

void createTracker(stCamShift* cs, const CvSize imgSize);
void releaseTracker(stCamShift* cs);
void startTracking(stCamShift* cs, IplImage* pImg, CvRect* pRect);
CvBox2D track(stCamShift* cs, IplImage*);

#endif /* CAMSHIFT_H */
