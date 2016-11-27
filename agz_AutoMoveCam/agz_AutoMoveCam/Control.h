#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

class Control{
private:
	int height;
	int width;

	// ロボットの現在位置
	cv::Point2i nowPoint;
	// ロボットの動作
	std::string action;
	// 内外判定結果
	std::string out;

	struct target{
		cv::Point2i point;
		int count;
	};

	// すべてのターゲット
	std::vector<target> allTarget;
	// 次に向かうターゲット
	std::vector<target>::iterator nowTarget_itr;

public:
	// Constructor
	Control(int width, int height);

	// ターゲットの更新
	void is_updateTarget(void);
	// ロボットの動作決定
	int robot_action(cv::Point2i Previous);
	// ターゲットエリアの訪問回数の更新
	void target_count(void);
	// 内外判定
	void is_out(void);
	// プロット
	void plot_target(cv::Mat img);

	//	set function
	void set_target(void);
	void set_point(cv::Point2i p);
};