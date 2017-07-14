#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "head_calibration.h"

void Result_Camera_Corner();
void Add_Chessboard_Points();
void Current_Camera_Corner(Mat input_image);//ʵʱ��ʾ
void Calibrate();
Mat Calibration_Remap(Mat image,int flag,int flag_0);
void Count_Error();
void Result_Print();
void Save_Data();
void PrintfVectorMat(vector<Mat> umatrix);
void PrintfMatrix(Mat umatrix);
void Read_Data();//��ȡ�����������Լ�У������
//void Show_Current_Time(int k);

#endif