#include"calibration.h"
#include"calibration_c.h"
#include "Init_Data.h"
#include "head_calibration.h"

void Help();
void main()
{
	system("color 9F");
	int num;
	printf("是否进行相机标定,选择1进行相机标定，选择2不进行相机标定:");
	scanf("%d", &num);
	switch (num)
	{
	case 1:
		printf("此时进行相机标定\n");
		Help();
		Show_Prepare();//进行校正准备
		Result_Camera_Corner();//C++标定,最终结果显示
		//Result_Camera_Corner_C();//C进行标定,显示
		printf("相机标定结束\n");
	case 2:
		printf("此时不进行相机标定\n");
		Show_Image();//进行实时校正
	default:
		break;
	}
	system("pause");
}
void Help()
{
	printf("\n");
	printf("默认图片数目为%d张\n",n_images);
	printf("默认图片视频分辨率为%d*%d\n",main_image_width,main_image_height);
	printf("默认格子宽度为%.2fcm\n",square_width);
	printf("默认图像X方向角点数为%d个,Y方向角点数为%d个\n",chessboard_size_width,chessboard_size_heigth);
}

