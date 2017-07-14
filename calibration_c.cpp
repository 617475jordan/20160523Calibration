#include "calibration_c.h"

extern int image_count;//实时图片总数
/***********************************/
//C程序中定义
int image_count0_c = 0;//corner找到的图片数量

//计算内参后数据保存
float intr[3][3] = { 0.0 };
float dist[4] = { 0.0 };
float tranv[3] = { 0.0 };
float rotv[3] = { 0.0 };

double total_error_value_c = 0;
double error_value_c[n_images];
double total_ave_error_c;

CvMat *camera_matrix = cvCreateMat(3, 3, CV_32FC1);//内参数
CvMat *dist_coeffs = cvCreateMat(1, 4, CV_32FC1);//畸变参数
CvMat *rvecs = cvCreateMat(image_count0_c, 3, CV_32FC1);//旋转向量
CvMat *tvecs = cvCreateMat(image_count0_c, 3, CV_32FC1);//平移向量

CvMat *point_counts = cvCreateMat(image_count0_c, 1, CV_32SC1);
CvMat *object_points = cvCreateMat(image_count0_c*n_points, 3, CV_32FC1);
CvMat *image_points = cvCreateMat(image_count0_c*n_points, 2, CV_32FC1);

//由于Iplimage *需要初始化，因此无法进行实时标定
void Result_Camera_Corner_C()
{
	CvPoint2D32f corners[n_points*n_images];//所有的角点坐标
	int corner_count[n_images] = { 0 };
	char filename[20];
	//创建图像
	IplImage     *chessboard_image = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//连接棋盘格角点图像
	IplImage     *current_frame_gray = cvCreateImage(photo_size, IPL_DEPTH_8U, 1); //连接棋盘格角点图像（缩小大小后的，便于显示）
	IplImage     *current_frame_rgb = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//灰度图像，用来提取角点
	IplImage     *currentsamall_frame_rgb = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//原始载入图像

	//当前图像序号
	for (int captured_frames = 0; captured_frames<image_count; captured_frames++)
	{
		sprintf(filename, "%d.jpg", captured_frames + 1);
		chessboard_image = cvLoadImage(filename, 1);
		int flag = 0;
		if (chessboard_image == NULL)
		{
			printf("load %d.jpg failed!\n", captured_frames + 1);
			captured_frames++;
		}
		cvCvtColor(chessboard_image, current_frame_gray, CV_BGR2GRAY);
		cvCopy(chessboard_image, current_frame_rgb, 0);
		//查找角点
		int find_corners_result;
		find_corners_result =
			cvFindChessboardCorners(current_frame_gray,//当前图像
			chessboard_size,//角度区域 棋盘图中每行和每列角点的个数
			&corners[image_count0_c*n_points],// 检测到的角点
			&corner_count[image_count0_c],//int corner_count[n_images] = {0};输出，角点的个数。如果不是NULL，函数将检测到的角点的个数存储于此变量。
			CV_CALIB_CB_ADAPTIVE_THRESH);//使用自适应阈值
		//通过迭代来发现具有子象素精度的角点位置
		cvFindCornerSubPix(current_frame_gray, //当前图像
			&corners[image_count0_c*n_points],//检测到的角点
			n_points, //角点的个数
			chessboard_size,
			cvSize(-1, -1), //不忽略corner临近的像素进行精确估计，
			cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 30, 0.01));
		//迭代次数（iteration）或者最小精度（epsilon）

		cvDrawChessboardCorners(current_frame_rgb, chessboard_size, //绘制检测到的棋盘角点
			&corners[image_count0_c*n_points],
			n_points,
			find_corners_result);
		if (find_corners_result != 0)
		{
			char ok_filename[20];
			sprintf(ok_filename, "%d.bmp", image_count0_c+1);
			cvSaveImage(ok_filename,chessboard_image);
			image_count0_c++;
			flag = 1;
		}

		if (flag == 1)
		{
			printf("当前显示图片为%d.jpg,使用本图片进行标定\n", captured_frames + 1);
		}
		if (flag == 0)
		{
			printf("当前显示图片为%d.jpg,不能使用本图片进行标定\n", captured_frames + 1);
		}
		cvNamedWindow(filename, CV_WINDOW_AUTOSIZE);
		cvShowImage(filename, current_frame_rgb);
		cvWaitKey(1000);
		destroyWindow(filename);
	}
	//标定的结果保存


	InitCorners3D(object_points, chessboard_size, image_count0_c, square_width);//把2维点转化成三维点（object_points输出量）
	cvSetData(image_points, corners, sizeof(CvPoint2D32f));//把corners 复制到 image_points中
	cvSetData(point_counts, &corner_count, sizeof(int));//把corner_count 复制到 point_counts中 每个图像中多少个角点
	printf("一共可以使用%d图片进行标定\n", image_count0_c);
	printf("开始进行相机标定\n");
	cvCalibrateCamera2(object_points,
		image_points,
		point_counts,//指定不同视图里点的数目
		photo_size,
		camera_matrix,
		dist_coeffs,
		rvecs,
		tvecs,
		0);
	printf("相机标定结束\n");
	//存储计算内参后的数据
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			intr[i][j] = ((float*)(camera_matrix->data.ptr + camera_matrix->step*i))[j];
		}
		dist[i] = ((float*)(dist_coeffs->data.ptr))[i];
		tranv[i] = ((float*)(tvecs->data.ptr))[i];
		rotv[i] = ((float*)(rvecs->data.ptr))[i];
	}
	dist[3] = ((float*)(dist_coeffs->data.ptr))[3];

	cvNamedWindow("Undistort_image", 1);
	IplImage     *srcimage = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//采集的单张彩色图像
	IplImage     *grayimage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);//转换为单张灰度图像
	IplImage     *result_image = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//要取角点的单张图像
	//显示校正后的图片
	printf("一共矫正了%d图片\n", image_count0_c);
	printf("校正后的图片....\n");
	for (int i = 1; i <= image_count0_c; i++)
	{
		sprintf(filename, "%d.bmp", i);
		chessboard_image = cvLoadImage(filename, 1);
		if (chessboard_image == NULL)
		{
			printf("load %d.bmp failed!\n", i + 1);
			i++;
		}
		if ((srcimage = cvLoadImage(filename, 1)) != 0)
		{
			//进行校正
			cvUndistort2(srcimage, result_image, camera_matrix, dist_coeffs);
			cvSaveImage("矫正后的图像.bmp", result_image);
			cvShowImage("Undistort_image", result_image);
			cvWaitKey(1000);
			destroyWindow("Undistort_image");
		}
	}
	printf("\n");
//	Save_Data_C();//数据保存
	//评价标定结果
	double total_err = 0.0;
	double err = 0.0;
	double point_cousum = 0;
	cout << "\t每幅图像的定标误差：\n";
	for (int i = 0; i<image_count0_c; i++)
	{
		//提取单张图像的参数 object_matrix rotation_matrix translation_matrix
		CvMat *object_matrix = cvCreateMat(n_points, 3, CV_32FC1);
		CvMat *image_matrix = cvCreateMat(n_points, 2, CV_32FC1);
		CvMat *project_image_matrix = cvCreateMat(n_points, 2, CV_32FC1);
		CvMat *rotation_matrix_1 = cvCreateMat(1, 3, CV_32FC1);
		CvMat *translation_matrix_1 = cvCreateMat(1, 3, CV_32FC1);
		CvMat *rotation_matrix = cvCreateMat(3, 1, CV_32FC1);
		CvMat *translation_matrix = cvCreateMat(3, 1, CV_32FC1);
		double err = 0;
		cvGetRows(object_points, object_matrix, i*n_points, (i + 1)*n_points, 1);
		cvGetRow(rvecs, rotation_matrix_1, i);
		cvReshape(rotation_matrix_1, rotation_matrix, 0, 3);
		cvGetRow(tvecs, translation_matrix_1, i);
		cvReshape(translation_matrix_1, translation_matrix, 0, 3);
		cvGetRows(image_points, project_image_matrix, i*n_points, (i + 1)*n_points, 1);

		//反向投影，投影到的点保存在image_matrix中
		cvProjectPoints2(object_matrix, rotation_matrix, translation_matrix,
			camera_matrix, dist_coeffs, image_matrix, 0, 0, 0, 0, 0);
		err = cvNorm(image_matrix, project_image_matrix, CV_L2, 0);
		//total_err = err*err;
		//point_cousum = point_cousum + (point_counts->data.ptr + point_counts->step*i)[0];
		//error_value_c[i] = sqrt(err*err / (point_counts->data.ptr + point_counts->step*i)[0]);
		error_value_c[i] = err / 36;
		total_err = total_err + error_value_c[i];
		printf("第%d幅图像的误差为%f\n", i + 1,error_value_c[i] );

	}
	printf("总的图像的平均误差为%f\n", total_err/image_count0_c);
	Result_Print_C();//输出标定结果
	system("pause");
	cvReleaseMat(&camera_matrix);
	cvReleaseMat(&dist_coeffs);
	cvReleaseMat(&rvecs);
	cvReleaseMat(&tvecs);
	cvReleaseMat(&point_counts);
	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvDestroyAllWindows();
}

//求取棋盘格上点的世界坐标
void InitCorners3D(CvMat *Corners3D, CvSize chessboard_size, int n_images, float SquareSize)
{
	int CurrentImage = 0;
	int CurrentRow = 0;
	int CurrentColumn = 0;
	int n_points = chessboard_size.height*chessboard_size.width;
	float * temppoints = new float[n_images*n_points * 3];

	for (CurrentImage = 0; CurrentImage < n_images; CurrentImage++)
	{
		for (CurrentRow = 0; CurrentRow < chessboard_size.height; CurrentRow++)
		{
			for (CurrentColumn = 0; CurrentColumn < chessboard_size.width; CurrentColumn++)
			{
				temppoints[(CurrentImage*n_points * 3) + (CurrentRow*chessboard_size.width +
					CurrentColumn) * 3] = (float)CurrentRow*SquareSize;
				temppoints[(CurrentImage*n_points * 3) + (CurrentRow*chessboard_size.width +
					CurrentColumn) * 3 + 1] = (float)CurrentColumn*SquareSize;
				temppoints[(CurrentImage*n_points * 3) + (CurrentRow*chessboard_size.width +
					CurrentColumn) * 3 + 2] = 0.f;
			}
		}
	}
	(*Corners3D) = cvMat(n_images*n_points, 3, CV_32FC1, temppoints);
}

//误差输出显示
void Result_Print_C()
{
	printf("-----------------------------------------\n ");
	printf("INTRINSIC MATRIX:  \n");
	printf("[ %6.4f %6.4f %6.4f ]  \n", intr[0][0], intr[0][1], intr[0][2]);
	printf("[ %6.4f %6.4f %6.4f ]  \n", intr[1][0], intr[1][1], intr[1][2]);
	printf("[ %6.4f %6.4f %6.4f ]  \n", intr[2][0], intr[2][1], intr[2][2]);
	printf("----------------------------------------- \n");
	printf("DISTORTION VECTOR:  \n");
	printf("[ %6.4f %6.4f %6.4f %6.4f ]  \n", dist[0], dist[1], dist[2], dist[3]);
	printf("----------------------------------------- \n");
	printf("ROTATION VECTOR(IMAGE1):  \n");
	printf("[ %6.4f %6.4f %6.4f ]  \n", rotv[0], rotv[1], rotv[2]);
	printf("TRANSLATION VECTOR(IMAGE1):  \n");
	printf("[ %6.4f %6.4f %6.4f ]  \n", tranv[0], tranv[1], tranv[2]);
	printf("----------------------------------------- \n");

	
}
void Save_Data_C()
{
	FileStorage fs_0;
	fs_0.open("Camera_Calibration_Data_C_0.xml", FileStorage::WRITE);
	fs_0 << "camera_matrix" << camera_matrix;
	fs_0 << "dist_coeffs" << dist_coeffs;
	fs_0.release();
}

void Read_Data_C()
{
	/*FileStorage fs_0;
	fs_0.open("Camera_Calibration_Data_C_0.xml", FileStorage::READ);
	if (!fs_0.isOpened())
	{
		printf("Datas couldn't be read\n");
		cvWaitKey(2000);
		exit(-1);
	}
	else
	{
		fs_0["camera_matrix"] >> camera_matrix;
		fs_0["dist_coeffs"] >> dist_coeffs;
		fs_0.release();
	}*/
}