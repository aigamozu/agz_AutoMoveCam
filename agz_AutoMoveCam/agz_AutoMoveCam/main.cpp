////////////////////////////////////////////////////////
//
//@comment : error C4996: 'localtime' ����������̂�,
//         �u�v���p�e�B�v-> �uC/C++�v-> �u�ڍאݒ�v->
//         �u�w��̌x���𖳎�����v�� [4996] ����͂��Ă�������.
//
//////////////////////////////////////////////////////////


#define GRAVITY 1 //@comment 0 : �摜���̗̈�  1: ���ڗ̈�
#include "Main.h"


using namespace std;
using namespace cv;

Mat test;

ofstream ofs(setFilename());
int main(int argc, char *argv[])
{

	LPCSTR com = "COM4";
	Img_Proc imp = Img_Proc();

	BYTE date = 1;


	//1.�|�[�g���I�[�v��
	arduino = CreateFile(com, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//2014/01/22�ǋL�@����łȂ���Ȃ��ꍇ�ɂ�"\\\\.\\COM7"�Ƃ���ƂȂ��邩������܂���B

	if (arduino == INVALID_HANDLE_VALUE) {
		cout << "PORT COULD NOT OPEN" << endl;
		cout << "PLEASE CHECK COM PORT NUMBER" << endl;
		system("PAUSE");
		exit(0);
	}
	//@comment ���c�̗̈�i���A�c�j����
	cout << "���c�̑傫������͂��Ă�������(m)�P��" << endl;
	cout << "�� : ";
	cin >> src_img_cols;
	cout << "�c : ";
	cin >> src_img_rows;

	src_img_cols *= 100;
	src_img_rows *= 100;

	if (src_img_cols < 300 || src_img_rows < 300) //@comment 3x3(m)�ȏ�̗̈���w��
	{
		cout << endl;
		cout << "�� �c�A�����ꂼ��R���ȏ���w�肵�Ă�������" << endl;
		cout << endl;
		system("PAUSE");
		exit(0);
	}

	//@comment �J�����̌Ăяo�� pc�̃J���� : 0 web�J���� : 1 (���ɂ���ĕω�)
	VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640); //@comment web�J�����̉�����ݒ�
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480); //@comment web�J�����̏c����ݒ�
	if (!cap.isOpened()) return -1; //@comment �Ăяo���~�X������ΏI��
	
	//@comment OptCam�p
	nm30_init();
	nm30_set_panorama_mode(1, 11);

	//@comment �n�߂̕��̃t���[���͈Â��\��������̂œǂݔ�΂�
	for (int i = 0; i < 10; i++) {
		cap >> src_frame; //@comment 1�t���[���擾
	}

	cout << "���c�̂S�_���N���b�N���Ă�������" << endl;

	//------------------���W�擾-----------------------------------------------
	//@comment �摜������}�E�X��4�_���擾���̌�ESC�L�[�������ƕϊ��������J�n����
	namedWindow("getCoordinates");
	imshow("getCoordinates", src_frame);
	//@comment �ϊ��������l�p�`�̎l���̍��W���Ƃ�(�N���b�N)
	cvSetMouseCallback("getCoordinates", getCoordinates, NULL);
	waitKey(0);
	destroyAllWindows();


	//------------------�����ϊ�-----------------------------------------------
	Point2f pts1[] = { Point2f(Ax, Ay), Point2f(Bx, By),
		Point2f(Cx, Cy), Point2f(Dx, Dy) };

	Point2f pts2[] = { Point2f(0, src_img_rows), Point2f(0, 0),
		Point2f(src_img_cols, 0), Point2f(src_img_cols, src_img_rows) };

	//@comment �����ϊ��s����v�Z
	Mat perspective_matrix = getPerspectiveTransform(pts1, pts2);
	Mat dst_img, colorExtra;

	//@comment �ϊ�(���`�⊮)
	warpPerspective(src_frame, dst_img, perspective_matrix, Size(src_img_cols, src_img_rows), INTER_LINEAR);

	//@comment �ϊ��O��̍��W��`��
	line(src_frame, pts1[0], pts1[1], Scalar(255, 0, 255), 2, CV_AA);
	line(src_frame, pts1[1], pts1[2], Scalar(255, 255, 0), 2, CV_AA);
	line(src_frame, pts1[2], pts1[3], Scalar(255, 255, 0), 2, CV_AA);
	line(src_frame, pts1[3], pts1[0], Scalar(255, 255, 0), 2, CV_AA);
	line(src_frame, pts2[0], pts2[1], Scalar(255, 0, 255), 2, CV_AA);
	line(src_frame, pts2[1], pts2[2], Scalar(255, 255, 0), 2, CV_AA);
	line(src_frame, pts2[2], pts2[3], Scalar(255, 255, 0), 2, CV_AA);
	line(src_frame, pts2[3], pts2[0], Scalar(255, 255, 0), 2, CV_AA);

	namedWindow("plotCoordinates", 1);
	imshow("plotCoordinates", src_frame);


	int frame = 0; //@comment �t���[�����ێ��ϐ�
	Mat plot_img;
	dst_img.copyTo(plot_img);
	//target_itr = allTarget.begin();
	int color_r = 0, color_g = 0, color_b = 0;
	int color_flag = 0;


	//4.���M
	char id = A;
	char command = 's';
	int key;

	// �t�@�C����������
	ofs << src_img_cols << ", " << src_img_rows << endl;
	ofs << "x��, y���i�␳�Ȃ��j, ypos�i�␳����j" << endl;

	Control control(src_img_cols, src_img_rows);//@comment �C���X�^���X����
	control.set_target();

	namedWindow("colorExt",1);
	createTrackbar("H","colorExt",&h_value,180);
	createTrackbar("S", "colorExt", &s_value, 255);
	createTrackbar("V", "colorExt", &v_value, 255);
	
	while (1) { //@comment ���샋�[�v

		cap >> src_frame;
		if (frame % 3 == 0) { //@comment�@�t���[���̎擾���𒲐߉\
			///////
			//2.����M�o�b�t�@������
			Ret = SetupComm(arduino, 1024, 1024);
			if (!Ret) {
				printf("SET UP FAILED\n");
				CloseHandle(arduino);
				system("PAUSE");
				exit(0);
			}
			Ret = PurgeComm(arduino, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			if (!Ret) {
				printf("CLEAR FAILED\n");
				CloseHandle(arduino);
				exit(0);
			}

			//3.��{�ʐM�����̐ݒ�
			DCB dcb;
			GetCommState(arduino, &dcb);
			dcb.DCBlength = sizeof(DCB);
			dcb.BaudRate = 57600;
			dcb.fBinary = TRUE;
			dcb.ByteSize = 8;
			dcb.fParity = NOPARITY;
			dcb.StopBits = ONESTOPBIT;

			Ret = SetCommState(arduino, &dcb);
			if (!Ret) {
				printf("SetCommState FAILED\n");
				CloseHandle(arduino);
				system("PAUSE");
				exit(0);
			}

			//�}�j���A�����[�h�ɕύX�R�}���h�̑��M
			//s:�X�^���o�C
			//m:�}�j���A��
			//a: �I�[�g

			key = waitKey(300);
			if (char(key) != -1){
				command = char(key);


				if (command == 's') {
					sentManualCommand(byte(0x00));
				
				}
				if (command == 'm') {
					sentManualCommand(byte(0x01));
					
				}
				if (command == 'a') {
					sentManualCommand(byte(0x01));
				
				}
			}
			//�p�P�b�g�쐬�E���M
			//command:�V�[�P���X�ԍ�0�`5
			else if (command >= '0' && command <= '9') {
				sentAigamoCommand(int(command - '0'));
			}

			//@comment �摜�����T�C�Y(�傫������ƃf�B�X�v���C�ɓ����Ȃ�����)
			resize(src_frame, test_image2, Size(src_img_cols, src_img_rows), CV_8UC3);
			//src_frame = imp.undist(src_frame); //@comment �J�����̘c�݂��Ƃ�(GoPro����)

			//--------------------�O���[�X�P�[����---------------------------------------

			//�ϊ�(���`�⊮)
			warpPerspective(src_frame, dst_img, perspective_matrix, Size(src_img_cols, src_img_rows), INTER_LINEAR);
			//@comment hsv�𗘗p���ĐԐF�𒊏o
			//���͉摜�A�o�͉摜�A�ϊ��Ah�ŏ��l�Ah�ő�l�As�ŏ��l�As�ő�l�Av�ŏ��l�Av�ő�l h:(0-180)���ۂ�1/2
			imp.colorExtraction(&dst_img, &colorExtra, CV_BGR2HSV, 0, h_value, s_value, 255, v_value, 255);
			//colorExtraction(&dst_img, &colorExtra, CV_BGR2HSV, 145, 165,70, 255, 70, 255);
			colorExtra.copyTo(extra_img);
			cvtColor(colorExtra, colorExtra, CV_BGR2GRAY);//@comment �O���[�X�P�[���ɕϊ�


			//�Q�l��
			//------------------�������l�ڑ��p--------------------------------------------
			Mat binari_2;

			//----------------------��l��-----------------------------------------------
			threshold(colorExtra, binari_2, 0, 255, THRESH_BINARY);
			dilate(binari_2, binari_2, element, Point(-1, -1), 3); //�c������3�� �Ō�̈����ŉ񐔂�ݒ�


			//---------------------�ʐόv�Z-----------------------------------------------
			//�擾�����̈�̒��ň�Ԗʐς̑傫�����̂�ΏۂƂ��Ă��̑Ώۂ̏d�S�����߂�B
			vector<vector<Point>> contours;
			findContours(binari_2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			double max_area = 0;
			int max_area_contour = -1;
			double x = 0;
			double y = 0;
			if (!contours.empty()){
				for (int j = 0; j < contours.size(); j++){
					double area = contourArea(contours.at(j));
					if (max_area < area){
						max_area = area;
						max_area_contour = j;
					}
				}
				int count = contours.at(max_area_contour).size();

				for (int k = 0; k < count; k++){
					x += contours.at(max_area_contour).at(k).x;
					y += contours.at(max_area_contour).at(k).y;
				}

				x /= count;
				y /= count;

				circle(extra_img, Point(x, y), 50, Scalar(0, 255, 255), 3, 4);
				circle(extra_img, Point(x, y), 5, Scalar(255, 255, 255), -1, CV_AA);
			}
			
			//---------------------�d�S�擾---------------------------------------------
			Point2i point;
			if (!GRAVITY)
			{
				point = imp.calculate_center(binari_2);//@comment moment�Ŕ��F�����̏d�S�����߂�

				//cout << "posion: " << point.x << " " << point.y << endl;//@comment �d�S�_�̕\��
			}
			else{
			point.x = x;
			point.y = y;
			}

			int ypos;
			int ydef = 0;
			int num = 0;
			if (point.x != 0) {
				ypos = src_img_rows - (point.y + 6 * ((1000 / point.y) + 1));
				ydef = src_img_rows - point.y;//@comment �␳�Ȃ����d�S
				cout << point.x << " " << ypos << endl; //@comment �ϊ��摜���ł̃��{�b�g�̍��W(�d�S)
				ofs << point.x << ", " << ydef << ", " << ypos << endl; //@comment �ϊ�
			}

			//---------------------���{�b�g�̓���擾------------------------------------
			//if (frame % 2 == 0){
			P1 = { point.x, src_img_rows - ydef };
			if (P1.x != 0 && P1.y != 0) {
				//line(dst_img, P1, P0[4], Scalar(255, 0, 0), 2, CV_AA);
				// �^�[�Q�b�g�̍X�V
				control.is_updateTarget();
				// ���݂̃��{�b�g�̈ʒu���̍X�V
				control.set_point(P1);
				// ���{�b�g�̓��쌈��
				action = control.robot_action(P0[4]);
				// �^�[�Q�b�g�̖K��񐔍X�V
				//num = control.target_count();
				
				control.heatmap(control.area_count(),heatmap_img);
				// ���O����
				control.is_out();

				for (int i = 1; i < 5; i++){
					P0[i] = P0[i - 1];
				}
			}
			else{
				action = 0;
			}
			P0[0] = P1;
			//}

			if (command == 'a'){
				cout << "send" << endl;
				sentAigamoCommand(action);
			}
			std::cout << "cmd " << int(command) << std::endl;

			
			//-------------------�d�S�_�̃v���b�g----------------------------------------- 
			if (!point.y == 0) { //@comment point.y == 0�̏ꍇ��exception���N����( 0���Z )
				//@comment �摜�C�~�̒��S���W�C���a�C�F(��)�C�������C���(-1, CV_AA�͓h��Ԃ�)
				circle(dst_img, Point(point.x, point.y), 8, Scalar(255, 255, 255), -1, CV_AA);
				circle(dst_img, Point(point.x, point.y + 6 * ((1000 / point.y) + 1)), 8, Scalar(0, 0, 0), -1, CV_AA);


				//@comment �d�S�_�̈ړ�����
				circle(plot_img, Point(point.x, point.y ), 8, Scalar(0, 0, 255), -1, CV_AA);
				if (waitKey(30) == 114) {
					namedWindow("plot_img", 1);
					imshow("plot_img", plot_img);
				}
			}

			//------------------�^�[�Q�b�g�̃v���b�g--------------------------------------
			control.plot_target(dst_img,P0[4]);


			//------------------�}�X�̃v���b�g--------------------------------------

			for (int i = 0; i <= src_img_cols; i += 100) {
				for (int j = 0; j <= src_img_rows; j += 100) {

					line(dst_img, Point(i, j), Point(i, src_img_cols), Scalar(200, 200, 200), 3);
					line(dst_img, Point(i, j), Point(src_img_rows, j), Scalar(200, 200, 200), 3);
				}
			}

			//------------------���i�̈�̃v���b�g--------------------------------------
			cv::Point2i A = { 100, src_img_rows - 100 }, B = { 100, 100 }, C = { src_img_cols - 100, 100 }, D = { src_img_cols - 100, src_img_rows - 100 };

			line(dst_img, Point(A), Point(B), Scalar(200, 0, 0), 3);
			line(dst_img, Point(B), Point(C), Scalar(200, 0, 0), 3);
			line(dst_img, Point(C), Point(D), Scalar(200, 0, 0), 3);
			line(dst_img, Point(D), Point(A), Scalar(200, 0, 0), 3);

			//---------------------�\������----------------------------------------------


			resize(dst_img, dst_img, Size(700, 700));
			resize(extra_img, extra_img, Size(700, 700));


			imshow("dst_image", dst_img);//@comment �o�͉摜
			imshow("colorExt", extra_img);//@comment �Ԓ��o�摜
			imshow("plot_img", plot_img);
			

			//@comment "q"����������v���O�����I��
			if (src_frame.empty() || waitKey(50) == 113)
			{
				destroyAllWindows();
				return 0;
			}
		}
		frame++;
	}

	ofs.close(); //@comment �t�@�C���X�g���[���̉��
	CloseHandle(arduino);
	system("PAUSE");
}
