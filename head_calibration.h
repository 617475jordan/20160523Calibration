#ifndef HEAD_CALIBRATION_H
#define HEAD_CALIBRATION_H

#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "stdlib.h"
#include <time.h>
#include <iostream>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <Windows.h>

using namespace cv;
using namespace std;

const int n_images = 8;//图片总数
const int main_image_width = 320 * 1;
const int main_image_height = 240 * 1;
const Size photo_size = Size(main_image_width, main_image_height);
const double    square_width = 17.2;//格子边长
const int chessboard_size_width = 6;//设置图像中X方向上的角点个数
const int chessboard_size_heigth = 4;//设置图像中Y方向上的角点个数
const Size chessboard_size = Size(chessboard_size_width, chessboard_size_heigth);
const int n_points = chessboard_size_width*chessboard_size_heigth;//单张图片中角点总数

#define Window_Name_0 "摄像头帧截取窗口"

#endif