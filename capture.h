#ifndef CAPTURE_H
#define CAPTURE_H

#include "cv.h"

int  initCapture();
void closeCapture();
IplImage* nextVideoFrame();

#endif /* CAPTURE_H */
