#ifndef CALIBRATION_C_H
#define CALIBRATION_C_H

#include "head_calibration.h"

void Result_Camera_Corner_C();
void InitCorners3D(CvMat *Corners3D, CvSize chessboard_size, int n_images, float SquareSize);//��ȡ���̸��ϵ����������
void Result_Print_C();
void Save_Data_C();
void Read_Data_C();
#endif