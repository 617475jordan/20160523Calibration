#include "Brightness_And_Contrast.h"

//全局变量函数描述
int g_nContrastValue=80;
int g_nBrightnessValue=80;
Mat g_srcimage;
Mat g_dstimage;
#define Window_Name_0 "摄像头帧截取窗口"
void  Brightness_And_Contrast_Init(Mat srcimage,double fps)
{
	g_srcimage  = srcimage;
	g_dstimage = Mat::zeros(g_srcimage.size(), g_srcimage.type());
	namedWindow(Window_Name_0, 1);
	//createTrackbar("对比度", Window_Name_0, &g_nBrightnessValue, 255, Brightness_And_Contrast);
	//createTrackbar("亮度", Window_Name_0, &g_nContrastValue, 255, Brightness_And_Contrast);
	Brightness_And_Contrast(g_nBrightnessValue,0,fps);
	Brightness_And_Contrast(g_nContrastValue, 0,fps);
}
void Brightness_And_Contrast(int, void *,double fps)
{
	int i, j, k;
	for (i = 0; i< g_srcimage.rows; i++)
	for (j = 0; j < g_srcimage.cols; j++)
	for (k = 0; k < 3; k++)
	{
		g_dstimage.at<Vec3b>(i, j)[k] =
			saturate_cast<uchar> ((g_nContrastValue*0.01)
			*(g_srcimage.at < Vec3b>(i, j)[k]) + g_nBrightnessValue);
	}

	char filename_1[20];
	sprintf(filename_1, "%.2f", fps);
	string fpsString("FPS:");
	fpsString = fpsString + filename_1;
	putText(g_dstimage, fpsString,
		Point(5, 20),
		FONT_HERSHEY_SIMPLEX,
		0.5, Scalar(0, 0));
	imshow(Window_Name_0, g_dstimage);
	cvWaitKey(10);
}