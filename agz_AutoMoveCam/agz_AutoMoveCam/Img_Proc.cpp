#include "Img_Proc.h"

//@comment コンストラクタ
Img_Proc::Img_Proc(){}

//@comment 重心取得用関数
cv::Point2i Img_Proc::calculate_center(cv::Mat *gray)
{
	cv::Point2i center = cv::Point2i(0, 0);
	cv::Moments moment = moments(*gray, true);

	if (moment.m00 != 0)
	{
		center.x = (int)(moment.m10 / moment.m00);
		center.y = (int)(moment.m01 / moment.m00);
	}

	return center;
}


//@comment カメラキャリブレーション用関数(gopro用)
cv::Mat Img_Proc::undist(cv::Mat src_img)
{
	cv::Mat dst_img;

	//@comment カメラマトリックス(gopro)
	cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 469.96, 0, 400, 0, 467.68, 300, 0, 0, 1);
	//@comment 歪み行列(gopro)
	cv::Mat distcoeffs = (cv::Mat_<double>(1, 5) << -0.18957, 0.037319, 0, 0, -0.00337);

	undistort(src_img, dst_img, cameraMatrix, distcoeffs);
	return dst_img;
}


//@comment 色抽出用関数 
void Img_Proc::colorExtraction(cv::Mat* src, cv::Mat* dst,
	int code,
	int ch1Lower, int ch1Upper, //@comment H(色相)　最小、最大
	int ch2Lower, int ch2Upper, //@comment S(彩度)　最小、最大
	int ch3Lower, int ch3Upper  //@comment V(明度)　最小、最大
	)
{
	cv::Mat colorImage;
	int lower[3];
	int upper[3];

	cv::Mat lut = cv::Mat(256, 1, CV_8UC3);

	cv::cvtColor(*src, colorImage, code);

	lower[0] = ch1Lower;
	lower[1] = ch2Lower;
	lower[2] = ch3Lower;

	upper[0] = ch1Upper;
	upper[1] = ch2Upper;
	upper[2] = ch3Upper;

	for (int i = 0; i < 256; i++) {
		for (int k = 0; k < 3; k++) {
			if (lower[k] <= upper[k]) {
				if ((lower[k] <= i) && (i <= upper[k])) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
			else {
				if ((i <= upper[k]) || (lower[k] <= i)) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
		}
	}
	//@comment LUTを使用して二値化
	cv::LUT(colorImage, lut, colorImage);

	//@comment Channel毎に分解
	std::vector<cv::Mat> planes;
	cv::split(colorImage, planes);

	//@comment マスクを作成
	cv::Mat maskImage;
	cv::bitwise_and(planes[0], planes[1], maskImage);
	cv::bitwise_and(maskImage, planes[2], maskImage);

	//@comemnt 出力
	cv::Mat maskedImage;
	src->copyTo(maskedImage, maskImage);
	*dst = maskedImage;

}

