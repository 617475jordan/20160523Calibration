#include "Init_Data.h"
#include "calibration.h"

vector<Point2f> current_corners;//���нǵ���
int image_count = 8;//��ǰ�ɹ���ȡͼƬ��Ŀ
//ͼƬ������Ŀ����Ƶ������Ŀ
int save_image_count = 0;
int save_video_count = 0;

Mat frame;

void Show_Prepare()
{
	system("color 9F");
	//��������ͷ
	int paused=0;
	VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, main_image_width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, main_image_height);
	//ȷ������ͷ
	if (!capture.isOpened())
	{
		printf("����ͷ��ʧ��\n");
		cvWaitKey(2000);
		exit(-1);
	}
	else
	{
		printf("����ͷ�򿪳ɹ�\n");
		namedWindow(Window_Name_0, WINDOW_AUTOSIZE || CV_WINDOW_FREERATIO);
		printf("ʵʱ�궨�����¡�S'����ȡ��ǰ֡������Ϊ�궨ͼƬ...\n");
		printf("���¡�A'���˳�\n");
		printf("���¡�p'����ͣ\n");
	}
	while (true)
	{
		char filename[20];
		capture >> frame;
		if (!frame.empty() && paused == 0)
		{
			Photo_Remap(frame);
			//Brightness_And_Contrast_Init(frame);
			Current_Camera_Corner(frame);//c++�½�������궨
			//imshow(Window_Name_0,frame);
		}
		if (cvWaitKey(1) == 's' || cvWaitKey(1) == 'S'&&image_count < n_images&&current_corners.size() == n_points)//���ֻ����ȲŻ�ȡ
		{
			sprintf_s(filename, "%d.jpg", image_count + 1);
			printf("��ǰ��ȡͼƬ:%d\n", image_count + 1);
			imwrite(filename, frame);
			image_count++;
		}
		if (cvWaitKey(1) == 'd' || cvWaitKey(1) == 'D')
		{
			sprintf_s(filename, "%d.jpg", image_count + 1);
			printf("��ǰ��ȡͼƬ:%d\n", image_count + 1);
			imwrite(filename, frame);
			image_count++;
		}
		if (cvWaitKey(1) == 'a' || cvWaitKey(1) == 'A' || image_count >= n_images)
		{
			frame.release();
			break;
		}
		if (cvWaitKey(1) == 'p' || cvWaitKey(1) == 'P')
		{
			paused++;
			if (paused == 2)
			{
				paused = 0;
				printf("��ʼ\n");
			}
			if (paused == 1)
			{
				printf("��ͣ\n");
			}
		}
	}
	capture.release();//�ر�����ͷ
	printf("����ͷ�ر�\n");
	printf("ͼƬ��ȡ���\n");
	printf("һ����ȡͼƬ%d\n", image_count);
	cvDestroyWindow(Window_Name_0);
}
void Show_Image()
{
	system("color 9F");
	VideoCapture capture(0);
	int paused = 0;
	double t0;
	double fps;
	capture.set(CV_CAP_PROP_FRAME_WIDTH, main_image_width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, main_image_height);
	if (!capture.isOpened())
	{
		printf("����ͷ��ʧ��\n");
		cvWaitKey(2000);
		exit(-1);
	}
	else
	{
		printf("����ͷ�򿪳ɹ�\n");
		printf("����'A'���˳�\n");
		printf("����'B'����ͼƬ����\n");
		printf("����'C'������Ƶ����\n");
		printf("���¡�p'����ͣ\n");
		namedWindow(Window_Name_0, WINDOW_AUTOSIZE || CV_WINDOW_FREERATIO);
	}
	Save_Data_Open();//ͼƬ����Ƶ����ȷ��
	while (true)
	{
		capture >> frame;
		t0 = (double)getTickCount();
		if (!frame.empty() && paused == 0)
		{
			Photo_Remap(frame);
			Calibration_Remap(frame, 0, 1);//���뱣֤����ͷ������ʵ��һ�£�����ʱ�����ʹ��ͼƬ������ͷ���ݣ���������ͷ����
		}
		if (cvWaitKey(1) == 'a' || cvWaitKey(1) == 'A')
		{
			//���ݱ���
			frame.release();
			destroyAllWindows();
			Save_Data_Close();
			break;
		}
		if (cvWaitKey(1) == 'b' || cvWaitKey(1) == 'B')
		{
			//���н�ͼ
			char filename[20];
			save_image_count++;
			sprintf(filename, "%d.jpg", save_image_count);
			printf("���ڱ���ͼƬ%d.jpg\n", save_image_count);
			imwrite(filename, frame);
		}
		if (cvWaitKey(1) == 'c' || cvWaitKey(1) == 'C')
		{
			//���б�����Ƶ
			char filename[20];
			save_video_count++;
			sprintf(filename, "%d.avi", save_image_count);
			printf("���ڱ�����Ƶ%d.avi\n", save_video_count);
			//VideoWriter;
			//�ж��Ѿ��������Ƶ�������Լ���Ƶ�Ƿ�ɾ��
		}
		if (cvWaitKey(1) == 'p' || cvWaitKey(1) == 'P')
		{
			paused++;
			if (paused == 2)
			{
				paused = 0;
				printf("��ʼ\n");
			}
			if (paused == 1)
			{
				printf("��ͣ\n");
			}
		}
		t0 = (double)(getTickCount() - t0) / getTickFrequency();
		fps = 1.0 / t0;
		char filename_1[20];
		sprintf(filename_1, "%.2f", fps);
		string fpsString("FPS:");
		fpsString = fpsString + filename_1;
		putText(frame, fpsString,
			Point(5, 20),
			FONT_HERSHEY_SIMPLEX,
			0.5, Scalar(0, 0));
		imshow(Window_Name_0, frame);
		cvWaitKey(1);
	}
	capture.release();//�ر�����ͷ
}

void Save_Data_Open()
{
	FileStorage file;
	file.open("Image_Viedo_Data.xml", FileStorage::READ);
	if (!file.isOpened())
	{
		printf("δ�ܵ���Image_Viedo_Data.xml\n");
		printf("���ݽ������³�ʼ��\n");
		save_image_count = 0;
		save_video_count = 0;
	}
	else
	{
		file["save_image_count"] >> save_image_count;
		file["save_video_count"] >> save_video_count;
		file.release();
	}
}
void Save_Data_Close()
{
	printf("�������ݱ���\n");
	FileStorage file;
	file.open("Image_Viedo_Data.xml", FileStorage::WRITE);
	file << "save_image_count" << save_image_count;
	file << "save_video_count" << save_video_count;
	file.release();
	printf("���ݱ������\n");
}
