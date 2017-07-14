#include "calibration.h"
#include "Init_Data.h"
#include "Brightness_And_Contrast.h"
extern vector<Point2f> current_corners;//ʵʱ�ǵ���
extern int image_count;//ʵʱͼƬ����
extern Mat Calibration_Remap_image;
vector<vector<Point2f>> image_points;//��⵽�ǵ���ͼ���еĶ��ض�ά����
vector<vector<Point3f>> object_points;//��⵽�ǵ���ʵ���еĶ�����ά����
int image_count0 = 0;//��ǰ�ҵ�ͼƬ����
// �������
Mat camera_matrix;
Mat dist_coeffs;
vector<Mat> rvecs, tvecs;
Mat map1, map2;// ����ͼ��ȥ����
bool mustInitUndistort; // �Ƿ����½���ȥ����
TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);

// �����ϵĵ����������
vector<Point2f> imageCorners;
vector<Point3f> objectCorners;

double total_error_value = 0;
double error_value[n_images];
double total_ave_error;

FILE *output_file;

void Result_Camera_Corner()
{
	Add_Chessboard_Points();// ������ͼ����ȡ�ǵ�
	Calibrate();// ����궨
	Count_Error();
	// ͼ��ȥ������ʾ
	cvWaitKey(1000);
	for (int i = 1; i <= image_count; i++)
	{
		char filename[20];
		sprintf_s(filename, "%d.jpg", i);
		Mat image = imread(filename);
		Calibration_Remap(image, i,0);
		image.release();
	}
	Result_Print();//�����ӡ���
	Save_Data();
	system("pause");
	destroyAllWindows();

}
// ������ͼ����ȡ�ǵ�
void Add_Chessboard_Points()
{
	for (int i = 0; i < chessboard_size_heigth; i++)
	{
		for (int j = 0; j < chessboard_size_width; j++)
		{
			objectCorners.push_back(Point3f(i, j, 0.0f));
		}
	}
	// ѭ������ͼƬ
	int current_image_count = 1;//���㵱ǰ��ȡͼƬ��Ŀ����ʼ��
	for (current_image_count; current_image_count <= image_count; current_image_count++)
	{
		Mat input_image; // ������������ͼ��
		Mat input_gray_image;
		char filename[20];
		bool find_corners_result;// �õ����̽ǵ�
		int success = 0;//��Ǹ���ͼƬ�Ƿ��ʺϱ궨
		sprintf(filename, "%d.jpg", current_image_count);
		input_image = imread(filename, 1);
		if (input_image.cols != main_image_width || input_image.rows != main_image_height)
		{
			printf("%d.jpgͼ��ֱ��ʲ�����\n",current_image_count);
			continue;
		}
		else
		{

			cvtColor(input_image, input_gray_image, CV_BGR2GRAY);
			find_corners_result = findChessboardCorners(input_image,
				chessboard_size,
				imageCorners,
				3);

			// ����ǵ���Ŀ����ҪҪ����ô������������
			if (imageCorners.size() == chessboard_size.area())
			{
				// ��ȡ�����ؾ���,���õ�������߾���
				cornerSubPix(input_gray_image,
					imageCorners,
					chessboard_size, // �������ڵ�һ���С
					Size(-1, -1), // ������һ���С��(-1, -1)��ʾû������
					criteria);
				// ���һ���ӽ��е�ͼ��㼰������
				image_points.push_back(imageCorners);//����ʱ�ǵ����������ͼ��ǵ�Ķ��ض�ά������
				object_points.push_back(objectCorners);//����ʱ�ǵ����������ʵ�ʽǵ�Ķ�����ά������
				image_count0++;
				success = 1;
			}
			// ���ƽǵ�,find_corners_result �Ѿ��ҵ��ǵ�
			drawChessboardCorners(input_image,
				chessboard_size,
				imageCorners,
				find_corners_result);

			// ��ʾ
			if (success == 1)
			{
				printf("��ǰ��ʾͼƬΪ%d.jpg,�ɽ��б궨\n", current_image_count);
			}
			if (success == 0)
			{
				printf("��ǰ��ʾͼƬΪ%d.jpg,���ɽ��б궨\n", current_image_count);
			}
			namedWindow(filename, WINDOW_AUTOSIZE);
			imshow(filename, input_image);
			cvWaitKey(1000);
			input_image.release();
			destroyWindow(filename);
		}
	}
}

void Current_Camera_Corner(Mat input_image)//ʵʱ��ʾ
{
	bool find_corners_result;
	Mat input_gray_image;
	cvtColor(input_image, input_gray_image, CV_BGR2GRAY);
	find_corners_result = findChessboardCorners(
		input_image,
		chessboard_size,
		current_corners,
		CV_CALIB_CB_ADAPTIVE_THRESH);
	// ����ǵ���Ŀ����ҪҪ����ô������������
	if (imageCorners.size() == chessboard_size.area())
	{
		// ��ȡ�����ؾ���,���õ�������߾���
		cornerSubPix(input_gray_image,
			imageCorners,
			chessboard_size, // �������ڵ�һ���С
			Size(-1, -1), // ������һ���С��(-1, -1)��ʾû������
			criteria);
	}
	namedWindow(Window_Name_0, WINDOW_AUTOSIZE);
	imshow(Window_Name_0, input_image);
}

// ���б궨��������ͶӰ���
// ����������ڲξ���(camera_matrix)
// �����˻���ϵ��(dist_coeffs)
// ��������ת����(rvecs)
// ������ƽ������(tvecs)
void Calibrate()
{

	if (image_count0 == 0)
	{
		printf("δ���ҵ�Cornersֵ������ʧ��\n");
		cvWaitKey(2000);
		exit(-1);
	}
	else
	{
		// �������½���ȥ����
		mustInitUndistort = true;
		printf("һ��ʹ��%d��ͼƬ���б궨\n", image_count0);
		printf("��ʼ��������궨\n");
		// ��ʼ�궨
		calibrateCamera(object_points, // 3D��
			image_points,  // ͼ���
			photo_size,    // ͼ��ߴ�
			camera_matrix, // ������������
			dist_coeffs,   // ����ϵ��
			rvecs, tvecs); // ��ת��ƽ��
	//	Save_Data();//����ϵ������
		printf("����궨����\n");
	}
}
//�������
void Count_Error()
{
	int flag = 0;
	
	for (int i = 0; i < image_count0; i++)
	{
		if (imageCorners.size() == chessboard_size.area())
		{
			vector<Point2f> image_points2;
			//������ת����,ƽ���������ڲξ�����������������ʵ�ʽǵ���ͼ���е�����
			projectPoints(object_points[i],//�ǵ�ʵ������
				rvecs[i],//��ת����
				tvecs[i],//ƽ������
				camera_matrix,//�ڲξ���
				dist_coeffs,//����������
				image_points2);//�洢ͼ��ʵ������
			Mat tempImagePointMat = Mat(1, image_points[i].size(), CV_32FC2);//�������ڴ���ǵ��ͼ������ľ���
			Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);//�������ڴ���ǵ��ͼ����������ֵ�ľ���  

			for (int j = 0; j < (int)image_points[i].size(); j++)
			{
				image_points2Mat.at<Vec2f>(0, j) = Vec2d(image_points2[j].x, image_points2[j].y);
				tempImagePointMat.at<Vec2f>(0, j) = Vec2d(image_points[i][j].x, image_points[i][j].y);
			}
			error_value[i] = norm(image_points2Mat, tempImagePointMat, NORM_L2);//����ֵ�������Թ�һ��;
			total_error_value += error_value[i] /= image_points[i].size();
			cout << "��" << flag + 1 << "��ͼ���ƽ�����:" << error_value[i] << "����" << endl;
			flag++;
		}
	}
	total_ave_error = total_error_value / flag;
	cout << "����ƽ�����:" << total_ave_error << "����" << endl;
	printf("�����Ϊ:%f\n", total_error_value);
	printf("%f\n", total_ave_error);
}
// �궨��ȥ��ͼ���еĻ���
Mat Calibration_Remap(Mat image, int flag, int flag_0)
{
	Mat undistorted;
	double undis_ave;
	if (flag_0 == 0)
		undis_ave = 1 - total_ave_error;
	if (flag_0 == 1)
	{
		Read_Data();
		undis_ave = 1 - total_ave_error;
		mustInitUndistort = true;
	}
	double undis_width = undis_ave*main_image_width;
	double undis_height = undis_ave*main_image_height;
	int undis_int_width = (int)undis_width;
	int undis_int_height = (int)undis_height;
	Size undis = Size(undis_int_width, undis_int_height);//У����ͼ��ֱ���
	if (mustInitUndistort)   // ÿ�α궨ֻ��Ҫ��ʼ��һ��
	{
		initUndistortRectifyMap(
			camera_matrix,  // CalibrateCamera�м���õ����������
			dist_coeffs,    // CalibrateCamera�м���õ��Ļ������
			Mat(),     // ��ѡ��У������(�˴�Ϊ��)
			Mat(),     // ��������undistorted������������
			undis, // image.size(), undistorted����ĳߴ�
			CV_32FC1,      // ���ӳ��ͼ�������
			map1, map2);   // x�����y����ӳ�亯��
		mustInitUndistort = false;
	}
	double border_larger_width,border_lager_height;
	border_larger_width = image.cols / undis_ave - image.cols;
	border_lager_height = image.rows / undis_ave - image.rows;
	Mat temp;
	copyMakeBorder(image, temp,
		0,
		border_larger_width/2,
		border_lager_height/2,
		border_lager_height/2,
		BORDER_CONSTANT, 0);
	//imshow(" ", temp);	
	// Ӧ��ӳ�亯��
	undistort( temp,undistorted, camera_matrix, dist_coeffs);
	/*remap( undistorted,temp, map1, map2,
		INTER_LINEAR);*/
	
    resize( undistorted, image,photo_size,INTER_LINEAR);//��ͼƬ�ָ���ԭʼ��С
	//imshow(" ", undistorted);
	//��ʾУ����ͼƬ
	char filename[20];
	if (flag_0 == 0)
	{
		sprintf(filename, "%d.bmp", flag);
		imwrite(filename, image);
		printf("��ǰУ��ͼƬΪ%d.bmp\n", flag);
		//Show_Current_Time(flag);
		imshow(filename, image);
		cvWaitKey(1000);
		undistorted.release();
	//	image.release();
		destroyWindow(filename);
		return image;
	}
	//ʵ�����ͼƬ
    if (flag_0 == 1)
	{
		namedWindow(Window_Name_0, WINDOW_AUTOSIZE);
	/*	vector<Mat> channels;
		split(undistorted, channels);
		imshow(" ", undistorted);
		for (int i = 0; i < channels.size(); i++)
		{
			equalizeHist(channels.at(i), channels.at(i));
		}
		merge(channels, undistorted);*/
		return image;
	//	Brightness_And_Contrast_Init(undistorted,fps); �ԱȶȺ����ȵ���
	}
}

//��Ҫ����������
void Result_Print()
{
	printf("����ڲξ���\n");
	PrintfMatrix(camera_matrix);                                         //����̨��ʾ����ڲξ���  

	printf("����������\n");
	PrintfMatrix(dist_coeffs);                                        //����̨��ʾ�������  

	printf("��ת�����飺\n");
	PrintfVectorMat(rvecs);                                                 //����̨��ʾ��ת������  

	printf("ƽ�������飺\n");
	PrintfVectorMat(tvecs);                                                //����̨��ʾƽ�������� 
	
}
//�ڲκͻ���������
void PrintfMatrix(Mat umatrix)
{
	for (int i = 0; i < umatrix.rows; ++i)
	{
		for (int j = 0; j < umatrix.cols; ++j)
		{
				printf("%lf ", umatrix.at<double>(i, j));
		}
		printf("\n");
	}
	printf("\n");
}
//��ת��ƽ�Ƽ������
void PrintfVectorMat(vector<Mat> umatrix)
{
	for (int i = 0; i < (int)umatrix.size(); ++i)
	{
		for (int j = 0; j < umatrix[i].rows; ++j)
		{
			for (int k = 0; k < umatrix[i].cols; ++k)
			{
					printf("  %lf", umatrix[i].at<double>(j, k));
			}
		}
		printf("\n");
	}
}

void Read_Data()
{
	FileStorage fs_0;
	fs_0.open("Camera_Calibration_Data.xml", FileStorage::READ);
	if (!fs_0.isOpened())
	{
		printf("��������������궨\n");
		printf("�˳�\n");
		exit(-1);
	}
	else
	{
		fs_0["camera_matrix"] >> camera_matrix;
		fs_0["dist_coeffs"] >> dist_coeffs;
		fs_0["total_ave_error"] >> total_ave_error;
		//fs_0["map1"] >> map1;
		//fs_0["map2"] > map2;
		fs_0.release();
	}
}

//���������xml�ļ���
void Save_Data()
{
	FileStorage fs_0;
	fs_0.open("Camera_Calibration_Data.xml", FileStorage::WRITE);
	fs_0 << "camera_matrix" << camera_matrix;//��������ھ���
	fs_0 << "dist_coeffs" << dist_coeffs;//�������ϵ��
	fs_0 << "total_ave_error" << total_ave_error;//�����������
	//fs_0 << "map1" << map1;
	//fs_0 << "map2" << map2;
	fs_0.release();

	FileStorage fs_1("Camera_Calibration_Data_0.xml", FileStorage::WRITE);
	//д����תʸ����
	fs_1 << "rvecs" << "[";
	for (int i = 0; i < image_count0; i++)
	{
		stringstream cvter;
		string name_str = "The_RvecsValue_of_", name_num;//����ַ���
		cvter << i;
		cvter >> name_num;
		name_str.append(name_num);
		name_str.append("The_Photo");
		fs_1 << "{:" << name_str << rvecs[i] << "}";
	}
	fs_1 << "]";
	//д��ƽ��ʸ����
	fs_1 << "tvecs" << "[";
	for (int i = 0; i < image_count0; i++)
	{
		stringstream cvter;
		string name_str = "The_TvecsValue_of_", name_num;//����ַ���
		cvter << i;
		cvter >> name_num;
		name_str.append(name_num);
		name_str.append("The_Photo");
		fs_1 << "{:" << name_str << tvecs[i] << "}";
	}
	fs_1 << "]";
	//д��ÿ��ͼ���
	fs_1 << "Error_of_Per_Image" << "[";
	for (int i = 0; i < image_count0; i++)
	{
		stringstream cvter;
		string name_str = "The_ErrorValue_of_", name_num;//����ַ���
		cvter << i;
		cvter >> name_num;
		name_str.append(name_num);
		name_str.append("The_Photo");
		fs_1 << "{:" << name_str << error_value[i] << "}";
	}
	fs_1 << "]";
	fs_1.release();
	//����
	FileStorage fs_2("Camera_Calibration_Data_1.xml", FileStorage::WRITE);
	//д��ǵ�ͼ������
	fs_2 << "Image_Corners_Coordinates" << "[";
	for (int i = 0; i < image_count0; i++)
	{
		fs_2 << "{:" << "Photo_num" << i << "coordinates" << "[";
		for (int j = 0; j < (int)imageCorners.size(); j++)
		{
			fs_2 << image_points[i][j];
		}
		fs_2 << "]" << "}";
	}
	fs_2 << "]";
	fs_2 << "Object_Corners_Coordinates" << "[";
	for (int i = 0; i < image_count0; i++)
	{
		fs_2 << "{:" << "Photo_num" << i << "coordinates" << "[";
		for (int j = 0; j < (int)imageCorners.size(); j++)
		{
			fs_2 << object_points[i][j];
		}
		fs_2 << "]" << "}";
	}
	fs_2 << "]";
	fs_2.release();
}
/*
void Show_Current_Time(int k)
{
	//��������
	CvFont font;
	double h_scale = 1.0;
	double w_scale = 1.0;
	int linewidth = 1;//�߿�
	//ʱ������
	time_t nowtime;
	struct tm *timeinformation;
	int hour, minute, sec;
	char buf[9] = "00:00;00";
	time(&nowtime);
	timeinformation = localtime(&nowtime);
	hour = timeinformation->tm_hour;
	minute = timeinformation->tm_min;
	sec = timeinformation->tm_sec;
	cvInitFont(&font,
		CV_FONT_HERSHEY_COMPLEX | CV_FONT_ITALIC,
		h_scale,
		w_scale,
		0,
		linewidth);
	if (k == 0)
	{
		putText(Window_Name_0,
			buf, cvPoint(16, 36),
			,font,
			cvScalar(255, 255, 255));
	}
	if (k != 0)
	{
		char filename[20];
		sprintf(filename, "%d.bmp", k);
		cvPutText(filename,
			buf, cvPoint(16, 36),
			&font,
			cvScalar(255, 255, 255));
	}
}*/