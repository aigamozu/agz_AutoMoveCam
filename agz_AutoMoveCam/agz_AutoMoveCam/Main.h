#include <iostream>
#include <fstream>
#include <windows.h>
#include "Img_Proc.h"
#include "Control.h"
#include "SSK.h"
#include <math.h>
#include <time.h>



using namespace std;
using namespace cv;

//@comment �֐��Q
void getCoordinates(int event, int x, int y, int flags, void* param);
void sentAigamoCommand(int command);
void sentManualCommand(byte command);


//���M���Xbee�̃A�h���X
byte const robotAddr[] = { byte(0x00), byte(0x13), byte(0xA2), byte(0x00), byte(0x40), byte(0x9E), byte(0xAE), byte(0xF7) };

//�e�����o�C�g
byte const  A = byte(0x41), B = byte(0x42), C = byte(0x43), D = byte(0x44), E = byte(0x45), F = byte(0x46),
G = byte(0x47), H = byte(0x48), I = byte(0x49), J = byte(0x4a), K = byte(0x4b), L = byte(0x4c),
M = byte(0x4d), N = byte(0x4e), O = byte(0x4f), P = byte(0x50), Q = byte(0x51), R = byte(0x52),
S = byte(0x53), T = byte(0x54), U = byte(0x55), V = byte(0x56), W = byte(0x57), X = byte(0x58),
Y = byte(0x59), Z = byte(0x5a);

//���[�h���Ƃ�left��right��pwm
byte const lPwm[] = { byte(0x00), byte(0x18), byte(0x10), byte(0x10), byte(0x08), byte(0x08), byte(0x10), byte(0x10), byte(0x0c), byte(0x0c) };
byte const rPwm[] = { byte(0x00), byte(0x18), byte(0x08), byte(0x08), byte(0x10), byte(0x10), byte(0x0c), byte(0x08), byte(0x0c), byte(0x08) };

int src_img_cols = 0; //@comment �ϊ��摜width
int src_img_rows = 0; //@comment �ϊ��摜height 


int Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
int Tr, Tg, Tb;

//@comment �摜�ϐ�
Mat image1;
Mat heatmap_img(Size(250,250),CV_8UC3,Scalar(255,255,255));
Mat src_img, src_frame, dst_img, colorExtra,extra_img, test_image1, test_image2;
Mat element = Mat::ones(3, 3, CV_8UC1); //@comment �ǉ��@3�~3�̍s��ŗv�f�͂��ׂ�1�@dilate�����ɕK�v�ȍs��
Mat heat_img(cv::Size(600,600), CV_8UC3, cv::Scalar(255, 255, 255));
// �t�@�C���o��

//@����o�͗p�ϐ�
const string  str = "test.avi";

Point2i target, P0[5] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }, P1 = { 0, 0 };
Point2i pre_point; // @comment Point�\����<int�^>

int action;
Point2i a, b, c, d;	//�����̈�̒��_
int width;
int depth;
int test_flag = 0;

bool Ret;
HANDLE arduino;


int h_value = 5;
int s_value = 70;
int v_value = 37;




void getCoordinates(int event, int x, int y, int flags, void* param)
{

	static int count = 0;
	switch (event) {
	case CV_EVENT_LBUTTONDOWN://@comment ���N���b�N�������ꂽ��

		if (count == 0) {
			Ax = x, Ay = y;
			cout << "Ax :" << x << ", Ay: " << y << endl;
		}
		else if (count == 1) {
			Bx = x, By = y;
			cout << "Bx :" << x << ", By: " << y << endl;
		}
		else if (count == 2) {
			Cx = x, Cy = y;
			cout << "Cx :" << x << ", Cy: " << y << endl;
		}
		else if (count == 3) {
			Dx = x, Dy = y;
			cout << "Dx :" << x << ", Dy: " << y << endl;
		}
		else {
			cout << "rgb(" << x << "," << y << ")  ";

			Vec3b target_color = src_frame.at<Vec3b>(y, x);
			uchar r, g, b;
			Tr = target_color[2];
			Tg = target_color[1];
			Tb = target_color[0];
			cout << "r:" << Tr << " g:" << Tg << " b:" << Tb << endl;
		}
		count++;
		break;
	default:
		break;
	}
}


//�p�P�b�g�쐬�E���M
//command:�V�[�P���X�ԍ�0�`5

void sentAigamoCommand(int command) {

	DWORD dwSendSize;
	DWORD dwErrorMask;
	byte checksum = 0;

	//�p�P�b�g����
	byte requestPacket[] = { byte(0x7E), byte(0x00), byte(0x1F), byte(0x10), byte(0x01),
		robotAddr[0], robotAddr[1], robotAddr[2], robotAddr[3],
		robotAddr[4], robotAddr[5], robotAddr[6], robotAddr[7],
		byte(0xFF), byte(0xFE), byte(0x00), byte(0x00), A, G, S,
		M, F, A, T, A, L, 1, lPwm[byte(command)], R, 1, rPwm[byte(command)], A, G, E, byte(0x00) };

	std::cout << command << std::endl;

	//�`�F�b�N�T���̌v�Z
	for (int i = 3; i < 34; i++) {
		checksum += requestPacket[i];
	}
	checksum = 0xFF - (checksum & 0x00FF);
	requestPacket[34] = byte(checksum);

	//�p�P�b�g�̑��M
	Ret = WriteFile(arduino, requestPacket, sizeof(requestPacket), &dwSendSize, NULL);

	if (!Ret) {
		printf("SEND FAILED\n");
		CloseHandle(arduino);
		system("PAUSE");
		exit(0);
	}

}



//�}�j���A�����[�h�ɕύX�R�}���h�̑��M
//8:�X�^���o�C
//9:�}�j���A��

void sentManualCommand(byte command) {

	DWORD dwSendSize;
	DWORD dwErrorMask;
	byte checksum = 0;

	//�p�P�b�g����
	byte requestPacket[] = { byte(0x7E), byte(0x00), byte(0x1A), byte(0x10), byte(0x01),
		robotAddr[0], robotAddr[1], robotAddr[2], robotAddr[3],
		robotAddr[4], robotAddr[5], robotAddr[6], robotAddr[7],
		byte(0xFF), byte(0xFE), byte(0x00), byte(0x00), A, G, S, C, F, A, T, A, command, A, G, E, byte(0x00) };

	//�`�F�b�N�T���̌v�Z
	for (int i = 3; i < 29; i++) {
		checksum += requestPacket[i];
	}
	checksum = 0xFF - (checksum & 0x00FF);
	requestPacket[29] = byte(checksum);

	//�p�P�b�g�̑��M
	Ret = WriteFile(arduino, requestPacket, sizeof(requestPacket), &dwSendSize, NULL);

	if (!Ret) {
		printf("SEND FAILED\n");
		CloseHandle(arduino);
		system("PAUSE");
		exit(0);
	}

}


//@comment �f�[�^�o�͗pcsv�t�@�C���@

string setFilename(){
	time_t now = time(NULL);
	struct tm * pnow = localtime(&now);
	char time[32];
	string c = ".csv";
	string data = "./data/";
	
	//@comment sprintf���g����int�^��string�ɕϊ�
	sprintf(time, "%d_%d_%d_%d_%d", pnow->tm_year + 1900, pnow->tm_mon + 1,
		pnow->tm_mday, pnow->tm_hour, pnow->tm_min);


	return data+time + c; //@comment �t�@�C����
}

