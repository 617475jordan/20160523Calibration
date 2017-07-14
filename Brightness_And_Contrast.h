#ifndef BRIGHTNESS_AND_CONTRAST_H
#define BRIGHTNESS_AND_CONTRAST_H

#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

void  Brightness_And_Contrast_Init(Mat srcimage,double fps);
void Brightness_And_Contrast(int, void *,double fps);
#endif 