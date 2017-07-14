#ifndef KALMAN_H
#define KALMAN_H

#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>

using namespace cv;

class Kalman{
public:
	static inline Point calcPoint(Point2f center, double R, double angle);
	static void help();
	void Kalman_Main();
};
#endif