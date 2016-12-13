#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <map>

class Control{
private:
	int height;
	int width;

	// ���{�b�g�̌��݈ʒu
	cv::Point2i nowPoint;
	// ���{�b�g�̓���
	std::string action;
	// ���O���茋��
	std::string out;

	struct target{
		cv::Point2i point;
		int n;
	};

	// ���ׂẴ^�[�Q�b�g
	std::vector<target> allTarget;
	// ���Ɍ������^�[�Q�b�g
	std::vector<target>::iterator nowTarget_itr;

	// �q�[�g�}�b�v�p�z�� ���{�b�g�̖K��񐔂��i�[����
	int** small_area;

public:
	// Constructor
	Control(int width, int height);

	// �^�[�Q�b�g�̍X�V
	void is_updateTarget(void);

	// ���{�b�g�̓��쌈��
	int robot_action(cv::Point2i Previous);
	
	// �^�[�Q�b�g�G���A�̖K��񐔂̍X�V
	int target_count(void);
	
	// ���̈��Ԃ̖K��񐔂̍X�V
	cv::Point2i area_count(void);

	// ���O����
	void is_out(void);
	
	// �v���b�g
	void plot_target(cv::Mat img, cv::Point2i Previous);

	void heatmap(cv::Point2i pos, cv::Mat img);

	//	set function
	void set_target(void);
	void set_point(cv::Point2i p);

	cv::Mat heatmap(cv::Mat src, cv::Mat dst, int x_flag, int y_flag, int count);
};