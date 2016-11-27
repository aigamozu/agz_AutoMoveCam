#include "Control.h"

/////////////////////////////////////////////////////////////////////////////////
//
//	Constructor
//	
////////////////////////////////////////////////////////////////////////////////
Control::Control(int w, int h){
	width = w;
	height = h;
}

/////////////////////////////////////////////////////////////////////////////////
//
//	is_updateTarget
//	
//	ロボットがターゲットの半径３０cm以内に入ったとき, 次のターゲットに移る.
////////////////////////////////////////////////////////////////////////////////
void Control::is_updateTarget(void){
	int dx = nowPoint.x - nowTarget_itr->point.x;
	int dy = nowPoint.y - nowTarget_itr->point.y;
	double d = sqrt(dx * dx + dy * dy);

	// ターゲットの半径３０cm以内の領域に入ったら訪問完了➔ターゲットを移す
	if (d < 30.0) {
		nowTarget_itr++;
	}

	// 最後のターゲットまで訪問したら最初のターゲットに戻る
	if (nowTarget_itr == allTarget.end()){
		nowTarget_itr = allTarget.begin();
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
//	robot_action
//	
//	ロボットの動作を決定する.
////////////////////////////////////////////////////////////////////////////////
int Control::robot_action(cv::Point2i Previous){
	cv::Point2i P0 = nowPoint - Previous;
	cv::Point2i P1 = nowTarget_itr->point - nowPoint;

	int dx1 = abs(nowPoint.x - 100);
	int dx2 = abs(nowPoint.x - height - 100);

	double angle = asin(P0.cross(P1) / (sqrt(P0.x * P0.x + P0.y * P0.y) * sqrt(P1.x * P1.x + P1.y * P1.y))) / CV_PI * 180;
	if (P0.dot(P1) >= 0) {
		if (P0.cross(P1) >= 0) {
			angle = 360.0 - angle;
		}
		else if (P0.cross(P1) < 0) {
			angle = -360.0 - angle;
		}
	}

	if (-30 < angle && angle < 30) {
		action = "f";
		return 1;
	}
	else if (angle <= -30) {
		if ((dx1 < 50.0) || (dx2 < 50.0)) {
			action = "r (large)";
			return 2;
		}
		else {
			action = "r (small)";
			return 3;
		}
	}
	else {
		if ((dx1 < 50.0) || (dx2 < 50.0)) {
			action = "l (large)";
			return 4;
		}
		else {
			action = "l (small)";
			return 5;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
//	target_count
//	
//	ロボットが内側領域内にいるかいないか調べる.
////////////////////////////////////////////////////////////////////////////////
void Control::target_count(void){
	int dx, dy;
	double d;
	for (std::vector<target>::iterator itr = allTarget.begin(); itr != allTarget.end(); itr++) {
		dx = nowPoint.x - itr->point.x;
		dy = nowPoint.y - itr->point.y;
		d = sqrt(dx * dx + dy * dy);

		if (d < 30.0){
			itr->count++;
			break;
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////
//
//	is_out
//	
//	ロボットが内側領域内にいるかいないか調べる.
////////////////////////////////////////////////////////////////////////////////
void Control::is_out(void){
	cv::Point2i A = { 100, height - 100 }, B = { 100, 100 }, C = { height - 100, 100 }, D = { height - 100, height - 100 };
	cv::Point2i BA = A - B, BC = C - B, BP = nowPoint - B;
	cv::Point2i DC = C - D, DA = A - D, DP = nowPoint - D;
	int c1, c2, c3, c4;
	bool flag1 = false, flag2 = false;

	c1 = BA.cross(BP);
	c2 = BP.cross(BC);
	c3 = DC.cross(DP);
	c4 = DP.cross(DA);

	if ((c1 >= 0 && c2 >= 0) || (c1 < 0 && c2 < 0)) {
		flag1 = true;
	}
	if ((c3 >= 0 && c4 >= 0) || (c3 < 0 && c4 < 0)) {
		flag2 = true;
	}
	if (flag1 && flag2) {
		out = "IN";
	}
	else out = "OUT";
}

/////////////////////////////////////////////////////////////////////////////////
//
//	plot_target
//	
//	ターゲットとロボットの状態をプロットする.
////////////////////////////////////////////////////////////////////////////////
void Control::plot_target(cv::Mat img){
	for (std::vector<target>::iterator itr = allTarget.begin(); itr != allTarget.end(); itr++) {
		cv::circle(img, cv::Point(itr->point), 28, cv::Scalar(255, 255, 0), 3, 4);
		cv::putText(img, std::to_string(itr->count), cv::Point(itr->point), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 0.5, CV_AA);
	}
	cv::circle(img, cv::Point(nowTarget_itr->point), 28, cv::Scalar(0, 0, 0), 3, 4);
	cv::putText(img, out, cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1.0, CV_AA);
	cv::putText(img, action, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1.0, CV_AA);
}


////////////////////////////////////////////////
//
//	set function
//
////////////////////////////////////////////////

void Control::set_target(void) {
	target t;

	allTarget.clear();

	int n = 0;
	for (int j = 0; j < (height / 100) - 2; j++) {
		if (j % 2 == 0) {
			for (int i = 0; i < (width / 100) - 2; i++) {
				t.point.x = (i + 1) * width / (width / 100) + 50;	t.point.y = height - (j + 1) * height / (height / 100) - 50;
				t.count = 0;
				allTarget.push_back(t);
				n++;
			}
		}
		else {
			for (int i = (width / 100) - 3; i >= 0; i--) {
				t.point.x = (i + 1) * width / (width / 100) + 50;	t.point.y = height - (j + 1) * height / (height / 100) - 50;
				t.count = 0;
				allTarget.push_back(t);
				n++;
			}
		}
	}

	nowTarget_itr = allTarget.begin();
}

void Control::set_point(cv::Point2i p){
	nowPoint = p;
}