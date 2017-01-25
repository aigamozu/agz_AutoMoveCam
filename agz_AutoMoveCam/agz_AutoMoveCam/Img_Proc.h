#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class Img_Proc
{
private:
	//int Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
	//int Tr, Tg, Tb;

public:
	Img_Proc();//@comment コンストラクタ
	cv::Point2i calculate_center(cv::Mat *);//@comment 中心の計算
	cv::Mat undist(cv::Mat); //@comment 歪み補正(gopro)
	//@comment 色抽出用関数
	void colorExtraction(cv::Mat* src, cv::Mat* dst,
		int code,
		int ch1Lower, int ch1Upper,
		int ch2Lower, int ch2Upper,
		int ch3Lower, int ch3Upper
		);

};