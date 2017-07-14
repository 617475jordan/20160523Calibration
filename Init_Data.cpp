#include "Init_Data.h"
#include "calibration.h"

vector<Point2f> current_corners;//所有角点数
int image_count = 8;//当前成功获取图片数目
//图片保存数目，视频保存数目
int save_image_count = 0;
int save_video_count = 0;

Mat frame;

void Show_Prepare()
{
	system("color 9F");
	//设置摄像头
	int paused=0;
	VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, main_image_width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, main_image_height);
	//确认摄像头
	if (!capture.isOpened())
	{
		printf("摄像头打开失败\n");
		cvWaitKey(2000);
		exit(-1);
	}
	else
	{
		printf("摄像头打开成功\n");
		namedWindow(Window_Name_0, WINDOW_AUTOSIZE || CV_WINDOW_FREERATIO);
		printf("实时标定：按下‘S'键截取当前帧并保存为标定图片...\n");
		printf("按下’A'键退出\n");
		printf("按下’p'键暂停\n");
	}
	while (true)
	{
		char filename[20];
		capture >> frame;
		if (!frame.empty() && paused == 0)
		{
			Photo_Remap(frame);
			//Brightness_And_Contrast_Init(frame);
			Current_Camera_Corner(frame);//c++下进行相机标定
			//imshow(Window_Name_0,frame);
		}
		if (cvWaitKey(1) == 's' || cvWaitKey(1) == 'S'&&image_count < n_images&&current_corners.size() == n_points)//如果只有相等才获取
		{
			sprintf_s(filename, "%d.jpg", image_count + 1);
			printf("当前获取图片:%d\n", image_count + 1);
			imwrite(filename, frame);
			image_count++;
		}
		if (cvWaitKey(1) == 'd' || cvWaitKey(1) == 'D')
		{
			sprintf_s(filename, "%d.jpg", image_count + 1);
			printf("当前获取图片:%d\n", image_count + 1);
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
				printf("开始\n");
			}
			if (paused == 1)
			{
				printf("暂停\n");
			}
		}
	}
	capture.release();//关闭摄像头
	printf("摄像头关闭\n");
	printf("图片获取完成\n");
	printf("一共获取图片%d\n", image_count);
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
		printf("摄像头打开失败\n");
		cvWaitKey(2000);
		exit(-1);
	}
	else
	{
		printf("摄像头打开成功\n");
		printf("按下'A'键退出\n");
		printf("按下'B'进行图片保存\n");
		printf("按下'C'进行视频保存\n");
		printf("按下’p'键暂停\n");
		namedWindow(Window_Name_0, WINDOW_AUTOSIZE || CV_WINDOW_FREERATIO);
	}
	Save_Data_Open();//图片，视频个数确认
	while (true)
	{
		capture >> frame;
		t0 = (double)getTickCount();
		if (!frame.empty() && paused == 0)
		{
			Photo_Remap(frame);
			Calibration_Remap(frame, 0, 1);//必须保证摄像头数据与实际一致，测试时候出现使用图片的摄像头数据，导致摄像头故障
		}
		if (cvWaitKey(1) == 'a' || cvWaitKey(1) == 'A')
		{
			//数据保存
			frame.release();
			destroyAllWindows();
			Save_Data_Close();
			break;
		}
		if (cvWaitKey(1) == 'b' || cvWaitKey(1) == 'B')
		{
			//进行截图
			char filename[20];
			save_image_count++;
			sprintf(filename, "%d.jpg", save_image_count);
			printf("正在保存图片%d.jpg\n", save_image_count);
			imwrite(filename, frame);
		}
		if (cvWaitKey(1) == 'c' || cvWaitKey(1) == 'C')
		{
			//进行保存视频
			char filename[20];
			save_video_count++;
			sprintf(filename, "%d.avi", save_image_count);
			printf("正在保存视频%d.avi\n", save_video_count);
			//VideoWriter;
			//判断已经保存的视频个数，以及视频是否被删除
		}
		if (cvWaitKey(1) == 'p' || cvWaitKey(1) == 'P')
		{
			paused++;
			if (paused == 2)
			{
				paused = 0;
				printf("开始\n");
			}
			if (paused == 1)
			{
				printf("暂停\n");
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
	capture.release();//关闭摄像头
}

void Save_Data_Open()
{
	FileStorage file;
	file.open("Image_Viedo_Data.xml", FileStorage::READ);
	if (!file.isOpened())
	{
		printf("未能导入Image_Viedo_Data.xml\n");
		printf("数据进行重新初始化\n");
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
	printf("进行数据保存\n");
	FileStorage file;
	file.open("Image_Viedo_Data.xml", FileStorage::WRITE);
	file << "save_image_count" << save_image_count;
	file << "save_video_count" << save_video_count;
	file.release();
	printf("数据保存完成\n");
}
