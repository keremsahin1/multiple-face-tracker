#ifndef FACEDET_H
#define FACEDET_H

#include "cv.h"

int initFaceDet(const char* sHaarCascadeFilePath);
void closeFaceDet();
int detectFaces(IplImage* pInputImg, CvRect** pFaceRects, int iMaxFaceCnt);

#endif /* FACEDET_H */
