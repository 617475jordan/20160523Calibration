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

const int n_images = 8;//ͼƬ����
const int main_image_width = 320 * 1;
const int main_image_height = 240 * 1;
const Size photo_size = Size(main_image_width, main_image_height);
const double    square_width = 17.2;//���ӱ߳�
const int chessboard_size_width = 6;//����ͼ����X�����ϵĽǵ����
const int chessboard_size_heigth = 4;//����ͼ����Y�����ϵĽǵ����
const Size chessboard_size = Size(chessboard_size_width, chessboard_size_heigth);
const int n_points = chessboard_size_width*chessboard_size_heigth;//����ͼƬ�нǵ�����

#define Window_Name_0 "����ͷ֡��ȡ����"

#endif