#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "head_calibration.h"

void Result_Camera_Corner();
void Add_Chessboard_Points();
void Current_Camera_Corner(Mat input_image);//实时显示
void Calibrate();
Mat Calibration_Remap(Mat image,int flag,int flag_0);
void Count_Error();
void Result_Print();
void Save_Data();
void PrintfVectorMat(vector<Mat> umatrix);
void PrintfMatrix(Mat umatrix);
void Read_Data();//读取保存的相机，以及校正数据
//void Show_Current_Time(int k);

#endif