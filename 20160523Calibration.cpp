#include"calibration.h"
#include"calibration_c.h"
#include "Init_Data.h"
#include "head_calibration.h"

void Help();
void main()
{
	system("color 9F");
	int num;
	printf("�Ƿ��������궨,ѡ��1��������궨��ѡ��2����������궨:");
	scanf("%d", &num);
	switch (num)
	{
	case 1:
		printf("��ʱ��������궨\n");
		Help();
		Show_Prepare();//����У��׼��
		Result_Camera_Corner();//C++�궨,���ս����ʾ
		//Result_Camera_Corner_C();//C���б궨,��ʾ
		printf("����궨����\n");
	case 2:
		printf("��ʱ����������궨\n");
		Show_Image();//����ʵʱУ��
	default:
		break;
	}
	system("pause");
}
void Help()
{
	printf("\n");
	printf("Ĭ��ͼƬ��ĿΪ%d��\n",n_images);
	printf("Ĭ��ͼƬ��Ƶ�ֱ���Ϊ%d*%d\n",main_image_width,main_image_height);
	printf("Ĭ�ϸ��ӿ��Ϊ%.2fcm\n",square_width);
	printf("Ĭ��ͼ��X����ǵ���Ϊ%d��,Y����ǵ���Ϊ%d��\n",chessboard_size_width,chessboard_size_heigth);
}

