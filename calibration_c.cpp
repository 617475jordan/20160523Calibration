#include "calibration_c.h"

extern int image_count;//ʵʱͼƬ����
/***********************************/
//C�����ж���
int image_count0_c = 0;//corner�ҵ���ͼƬ����

//�����ڲκ����ݱ���
float intr[3][3] = { 0.0 };
float dist[4] = { 0.0 };
float tranv[3] = { 0.0 };
float rotv[3] = { 0.0 };

double total_error_value_c = 0;
double error_value_c[n_images];
double total_ave_error_c;

CvMat *camera_matrix = cvCreateMat(3, 3, CV_32FC1);//�ڲ���
CvMat *dist_coeffs = cvCreateMat(1, 4, CV_32FC1);//�������
CvMat *rvecs = cvCreateMat(image_count0_c, 3, CV_32FC1);//��ת����
CvMat *tvecs = cvCreateMat(image_count0_c, 3, CV_32FC1);//ƽ������

CvMat *point_counts = cvCreateMat(image_count0_c, 1, CV_32SC1);
CvMat *object_points = cvCreateMat(image_count0_c*n_points, 3, CV_32FC1);
CvMat *image_points = cvCreateMat(image_count0_c*n_points, 2, CV_32FC1);

//����Iplimage *��Ҫ��ʼ��������޷�����ʵʱ�궨
void Result_Camera_Corner_C()
{
	CvPoint2D32f corners[n_points*n_images];//���еĽǵ�����
	int corner_count[n_images] = { 0 };
	char filename[20];
	//����ͼ��
	IplImage     *chessboard_image = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//�������̸�ǵ�ͼ��
	IplImage     *current_frame_gray = cvCreateImage(photo_size, IPL_DEPTH_8U, 1); //�������̸�ǵ�ͼ����С��С��ģ�������ʾ��
	IplImage     *current_frame_rgb = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//�Ҷ�ͼ��������ȡ�ǵ�
	IplImage     *currentsamall_frame_rgb = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//ԭʼ����ͼ��

	//��ǰͼ�����
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
		//���ҽǵ�
		int find_corners_result;
		find_corners_result =
			cvFindChessboardCorners(current_frame_gray,//��ǰͼ��
			chessboard_size,//�Ƕ����� ����ͼ��ÿ�к�ÿ�нǵ�ĸ���
			&corners[image_count0_c*n_points],// ��⵽�Ľǵ�
			&corner_count[image_count0_c],//int corner_count[n_images] = {0};������ǵ�ĸ������������NULL����������⵽�Ľǵ�ĸ����洢�ڴ˱�����
			CV_CALIB_CB_ADAPTIVE_THRESH);//ʹ������Ӧ��ֵ
		//ͨ�����������־��������ؾ��ȵĽǵ�λ��
		cvFindCornerSubPix(current_frame_gray, //��ǰͼ��
			&corners[image_count0_c*n_points],//��⵽�Ľǵ�
			n_points, //�ǵ�ĸ���
			chessboard_size,
			cvSize(-1, -1), //������corner�ٽ������ؽ��о�ȷ���ƣ�
			cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 30, 0.01));
		//����������iteration��������С���ȣ�epsilon��

		cvDrawChessboardCorners(current_frame_rgb, chessboard_size, //���Ƽ�⵽�����̽ǵ�
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
			printf("��ǰ��ʾͼƬΪ%d.jpg,ʹ�ñ�ͼƬ���б궨\n", captured_frames + 1);
		}
		if (flag == 0)
		{
			printf("��ǰ��ʾͼƬΪ%d.jpg,����ʹ�ñ�ͼƬ���б궨\n", captured_frames + 1);
		}
		cvNamedWindow(filename, CV_WINDOW_AUTOSIZE);
		cvShowImage(filename, current_frame_rgb);
		cvWaitKey(1000);
		destroyWindow(filename);
	}
	//�궨�Ľ������


	InitCorners3D(object_points, chessboard_size, image_count0_c, square_width);//��2ά��ת������ά�㣨object_points�������
	cvSetData(image_points, corners, sizeof(CvPoint2D32f));//��corners ���Ƶ� image_points��
	cvSetData(point_counts, &corner_count, sizeof(int));//��corner_count ���Ƶ� point_counts�� ÿ��ͼ���ж��ٸ��ǵ�
	printf("һ������ʹ��%dͼƬ���б궨\n", image_count0_c);
	printf("��ʼ��������궨\n");
	cvCalibrateCamera2(object_points,
		image_points,
		point_counts,//ָ����ͬ��ͼ������Ŀ
		photo_size,
		camera_matrix,
		dist_coeffs,
		rvecs,
		tvecs,
		0);
	printf("����궨����\n");
	//�洢�����ڲκ������
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
	IplImage     *srcimage = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//�ɼ��ĵ��Ų�ɫͼ��
	IplImage     *grayimage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);//ת��Ϊ���ŻҶ�ͼ��
	IplImage     *result_image = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);//Ҫȡ�ǵ�ĵ���ͼ��
	//��ʾУ�����ͼƬ
	printf("һ��������%dͼƬ\n", image_count0_c);
	printf("У�����ͼƬ....\n");
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
			//����У��
			cvUndistort2(srcimage, result_image, camera_matrix, dist_coeffs);
			cvSaveImage("�������ͼ��.bmp", result_image);
			cvShowImage("Undistort_image", result_image);
			cvWaitKey(1000);
			destroyWindow("Undistort_image");
		}
	}
	printf("\n");
//	Save_Data_C();//���ݱ���
	//���۱궨���
	double total_err = 0.0;
	double err = 0.0;
	double point_cousum = 0;
	cout << "\tÿ��ͼ��Ķ�����\n";
	for (int i = 0; i<image_count0_c; i++)
	{
		//��ȡ����ͼ��Ĳ��� object_matrix rotation_matrix translation_matrix
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

		//����ͶӰ��ͶӰ���ĵ㱣����image_matrix��
		cvProjectPoints2(object_matrix, rotation_matrix, translation_matrix,
			camera_matrix, dist_coeffs, image_matrix, 0, 0, 0, 0, 0);
		err = cvNorm(image_matrix, project_image_matrix, CV_L2, 0);
		//total_err = err*err;
		//point_cousum = point_cousum + (point_counts->data.ptr + point_counts->step*i)[0];
		//error_value_c[i] = sqrt(err*err / (point_counts->data.ptr + point_counts->step*i)[0]);
		error_value_c[i] = err / 36;
		total_err = total_err + error_value_c[i];
		printf("��%d��ͼ������Ϊ%f\n", i + 1,error_value_c[i] );

	}
	printf("�ܵ�ͼ���ƽ�����Ϊ%f\n", total_err/image_count0_c);
	Result_Print_C();//����궨���
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

//��ȡ���̸��ϵ����������
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

//��������ʾ
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