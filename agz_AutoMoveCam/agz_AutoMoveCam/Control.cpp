﻿#include "Control.h"

#define PROGRAM 1


/////////////////////////////////////////////////////////////////////////////////
//
//	Constructor
//	
////////////////////////////////////////////////////////////////////////////////
Control::Control(int w, int h){
	width = w;
	height = h;

	// ヒートマップ用配列のメモリ動的確保
	small_area = new int*[width / 100 * 5];
	for (int i = 0; i < width / 100 * 5; i++){
		small_area[i] = new int[height / 100 * 5];
	}

	// 配列の初期化
	for (int i = 0; i < width / 100 * 5; i++){
		for (int j = 0; j < height / 100 * 5; j++){
			small_area[i][j] = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
//	is_updateTarget
//	
//	ロボットがターゲットの半径３０cm以内に入ったとき, 次のターゲットに移る.
////////////////////////////////////////////////////////////////////////////////
void Control::is_updateTarget(void){

	if (PROGRAM == 0){
		int dx = nowPoint.x - nowTarget_itr->point.x;
		int dy = nowPoint.y - nowTarget_itr->point.y;
		double d = sqrt(dx * dx + dy * dy);

		// ターゲットの半径３０cm以内の領域に入ったら訪問完了➔ターゲットを移す
		if (d < 30.0) {
			nowTarget_itr++;
		}
	}

	if (PROGRAM == 1){
		// 右側のターゲットに向かっている場合
		if (nowTarget_itr->n % 2){
			// 右側のターゲット区間に入ったとき
			if (width - 200 < nowPoint.x && nowPoint.x < width - 100){
				if (50 < nowPoint.y && nowPoint.y < height - 50){
					// ターゲットの更新
					nowTarget_itr++;
				}
			}
		}
		// 左側のターゲットに向かっている場合
		else{
			// 右側のターゲット区間に入ったとき
			if (100 < nowPoint.x && nowPoint.x < 200){
				if (50 < nowPoint.y && nowPoint.y < height - 50){
					// ターゲットの更新
					nowTarget_itr++;
				}
			}
		}
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

	// ベクトル
	cv::Point2i P0 = nowPoint - Previous;
	cv::Point2i P1 = nowTarget_itr->point - nowPoint;

	// P0とP1のなす角を内積を用いて求める
	double angle = acos(P0.dot(P1) / (sqrt(P0.x * P0.x + P0.y * P0.y) * sqrt(P1.x * P1.x + P1.y * P1.y))) / CV_PI * 180;

	// ロボットの進行方向に対してターゲットが左方向にあるとき
	if (P0.cross(P1) < 0) {
		angle = -angle;
	}

	// ロボットの進行方向に対して前方向にターゲットがある
	if (-30 < angle && angle < 30) {
		action = "f";
		return 1;
	}
	// ロボットの進行方向に対して右方向にターゲットがある
	else if (angle >= 30) {
		action = "r";
		return 2;
	}
	// ロボットの進行方向に対して左方向にターゲットがある
	else {
		action = "l";
		return 4;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
//	area_count
//	
//	ロボットの訪問回数を求める.
////////////////////////////////////////////////////////////////////////////////
cv::Point2i Control::area_count(void){
	
	cv::Point2i p;
	// 位置情報 nowPointから配列の添え字番号を求める
	for (int i = 0; i < width / 100 * 5; i++){
		if (i * 20 <= nowPoint.x && nowPoint.x < (i + 1) * 20){
			p.x = i;
			break;
		}
	}
	for (int j = 0; j < height / 100 * 5; j++){
		if (j * 20 <= nowPoint.y && nowPoint.y < (j + 1) * 20){
			p.y = j;
			break;
		}
	}
	// 訪問回数の更新
	small_area[p.x][p.y] ++;
	return p;
}

/////////////////////////////////////////////////////////////////////////////////
//
//	is_out
//	
//	ロボットが内側領域内にいるかいないか調べる.
////////////////////////////////////////////////////////////////////////////////
void Control::is_out(void){
	// 四隅の座標
	cv::Point2i A = { 100, height - 100 }, B = { 100, 100 }, C = { height - 100, 100 }, D = { height - 100, height - 100 };
	// ベクトル
	cv::Point2i BA = A - B, BC = C - B, BP = nowPoint - B;
	cv::Point2i DC = C - D, DA = A - D, DP = nowPoint - D;

	int c1, c2, c3, c4;
	bool flag1 = false, flag2 = false;

	// 外積の計算
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
void Control::plot_target(cv::Mat img, cv::Point2i Previous){

	// すべてのターゲットのプロット（水色）
	for (std::vector<target>::iterator itr = allTarget.begin(); itr != allTarget.end(); itr++) {

		cv::circle(img, cv::Point(itr->point), 28, cv::Scalar(255, 255, 0), 3, 4);
		cv::putText(img, std::to_string(itr->n), cv::Point(itr->point), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 0.5, CV_AA);
	}

	// 現在向かうべきターゲットのプロット（黒）
	cv::circle(img, cv::Point(nowTarget_itr->point), 28, cv::Scalar(0, 0, 0), 3, 4);

	line(img, nowPoint, Previous, cv::Scalar(255, 0, 0), 2, CV_AA);

	// 内外判定結果の表示
	cv::putText(img, out, cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1.0, CV_AA);

	// ロボットの動作の表示
	cv::putText(img, action, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1.0, CV_AA);
}

/////////////////////////////////////////////////////////////////////////////////
//
//	heatmap
//	
//	ヒートマップ作成
////////////////////////////////////////////////////////////////////////////////

void Control::heatmap(cv::Point2i pos, cv::Mat img){

	//	１マスのピクセル数 10x10
	int size_x = 10 * 250 / width, size_y = 10 * 250 / height;
	int max_count = 150;

	// 配列の添え字番号から座標を求め、２で割る
	int x = (pos.x * 20 + 10) * 250 / width;
	int y = (pos.y * 20 + 10) * 250 / height;
	// 訪問回数
	int count = small_area[pos.x][pos.y];

	// カウントの最大値の更新
	if (count > max_count){
		max_count = count;
	}

	// 画像のサイズ分だけループを回す
	for (int i = y - size_y; i < y + size_y; i++){
		for (int j = x - size_x; j < x + size_x; j++){

			// countを色相に変換
			int brightness = count;
			float h = 240.0 - 240.0 / max_count * (float)brightness;

			// HSV -> BGR に変換
			float s = 1.0;
			float v = 1.0;
			const float u = v * 255.0;
			const int id = (int)floor(h / 60.0) % 6;
			const float fr = (h / 60.0) - id;
			const float p = u * (1.0f - s);
			const float q = u * (1.0f - s*fr);
			const float t = u * (1.0f - s*(1.0f - fr));

			switch (id){
			case 0:
				img.at<cv::Vec3b>(i, j)[0] = p;
				img.at<cv::Vec3b>(i, j)[1] = t;
				img.at<cv::Vec3b>(i, j)[2] = u;
				break;
			case 1:
				img.at<cv::Vec3b>(i, j)[0] = p;
				img.at<cv::Vec3b>(i, j)[1] = u;
				img.at<cv::Vec3b>(i, j)[2] = q;
				break;
			case 2:
				img.at<cv::Vec3b>(i, j)[0] = t;
				img.at<cv::Vec3b>(i, j)[1] = u;
				img.at<cv::Vec3b>(i, j)[2] = p;
				break;
			case 3:
				img.at<cv::Vec3b>(i, j)[0] = u;
				img.at<cv::Vec3b>(i, j)[1] = q;
				img.at<cv::Vec3b>(i, j)[2] = p;
				break;
			case 4:
				img.at<cv::Vec3b>(i, j)[0] = u;
				img.at<cv::Vec3b>(i, j)[1] = p;
				img.at<cv::Vec3b>(i, j)[2] = t;
				break;
			default:
				img.at<cv::Vec3b>(i, j)[0] = q;
				img.at<cv::Vec3b>(i, j)[1] = p;
				img.at<cv::Vec3b>(i, j)[2] = u;
				break;

			}
		}
	}

	// 区画のプロット
	for (int i = 0; i <= width; i += 100) {
		for (int j = 0; j <= height; j += 100) {
			line(img, cv::Point(i*250/width, j*250/height), cv::Point(i*250/width, 250), cv::Scalar(200, 200, 200), 3);
			line(img, cv::Point(i*250/width, j*250/height), cv::Point(250, j*250/height), cv::Scalar(200, 200, 200), 3);
		}
	}
	cv::namedWindow("heatmap", 1);
	cv::resize(img,img,cv::Size(700,700));
	cv::imshow("heatmap", img);
	std::cout << "heatmap" << std::endl;
}


////////////////////////////////////////////////
//
//	set function
//
////////////////////////////////////////////////

void Control::set_target(void) {
	target t;

	allTarget.clear();

	int num = 0;
	if (PROGRAM == 0){
		for (int j = 0; j < (height / 100) - 2; j++) {
			// 左から右へターゲットを設定する
			if (j % 2 == 0) {
				for (int i = 0; i < (width / 100) - 2; i++) {
					t.point = { (i + 1) * width / (width / 100) + 50, height - (j + 1) * height / (height / 100) - 50 };
					t.n = num;
					allTarget.push_back(t);
					num++;
				}
			}
			// 右から左へターゲットを設定する
			else {
				for (int i = (width / 100) - 3; i >= 0; i--) {
					t.point = { (i + 1) * width / (width / 100) + 50, height - (j + 1) * height / (height / 100) - 50 };
					t.n = num;
					allTarget.push_back(t);
					num++;
				}
			}
		}
	}

	if (PROGRAM == 1){
		for (int i = 0; i < (height / 100) - 2; i++){
			// 左側のターゲット設定
			t.point = { 150, height - (i + 1) * height / (height / 100) - 50 };
			t.n = num;
			allTarget.push_back(t);
			num++;

			// 右側のターゲット設定
			t.point = { width - 150, height - (i + 1) * height / (height / 100) - 50 };
			t.n = num;
			allTarget.push_back(t);
			num++;
		}
	}

	nowTarget_itr = allTarget.begin();
}

void Control::set_point(cv::Point2i p){
	nowPoint = p;
}