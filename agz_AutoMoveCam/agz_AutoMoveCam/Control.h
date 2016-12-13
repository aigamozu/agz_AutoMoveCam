#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <map>

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
		int n;
	};

	// すべてのターゲット
	std::vector<target> allTarget;
	// 次に向かうターゲット
	std::vector<target>::iterator nowTarget_itr;

	// ヒートマップ用配列 ロボットの訪問回数を格納する
	int** small_area;

public:
	// Constructor
	Control(int width, int height);

	// ターゲットの更新
	void is_updateTarget(void);

	// ロボットの動作決定
	int robot_action(cv::Point2i Previous);
	
	// ターゲットエリアの訪問回数の更新
	int target_count(void);
	
	// 小領域区間の訪問回数の更新
	cv::Point2i area_count(void);

	// 内外判定
	void is_out(void);
	
	// プロット
	void plot_target(cv::Mat img, cv::Point2i Previous);

	void heatmap(cv::Point2i pos, cv::Mat img);

	//	set function
	void set_target(void);
	void set_point(cv::Point2i p);

	cv::Mat heatmap(cv::Mat src, cv::Mat dst, int x_flag, int y_flag, int count);
};