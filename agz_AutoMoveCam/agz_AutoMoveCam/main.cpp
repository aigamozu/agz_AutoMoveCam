////////////////////////////////////////////////////////
//
//@comment : error C4996: 'localtime' が発生するので,
//         「プロパティ」-> 「C/C++」-> 「詳細設定」->
//         「指定の警告を無視する」に [4996] を入力してください.
//
//////////////////////////////////////////////////////////


#define GRAVITY 1 //@comment 0 : 画像中の領域  1: 注目領域
#include "Main.h"


using namespace std;
using namespace cv;

Mat test;

ofstream ofs(setFilename());


int main(int argc, char *argv[])
{
	//////////////////////////////////////////////////////
	//@comment カラーバー作成
	//
	/////////////////////////////////////////////////////
	int w = 500;
	int h = 50;
	cv::Mat_<cv::Vec3b> bar(h, w);
	for (int j = 0; j<h; j++)
	{
		for (int i = 0; i<w; i++)
		{
			bar(j, i) = calcPseudoColor(double(i) / (w - 1));
		}
	}
	
	LPCSTR com = "COM1";
	Img_Proc imp = Img_Proc();

	BYTE date = 1;


	//1.ポートをオープン
	arduino = CreateFile(com, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//2014/01/22追記　これでつながらない場合には"\\\\.\\COM7"とするとつながるかもしれません。

	if (arduino == INVALID_HANDLE_VALUE) {
		cout << "PORT COULD NOT OPEN" << endl;
		cout << "PLEASE CHECK COM PORT NUMBER" << endl;
		system("PAUSE");
		exit(0);
	}
	//@comment 水田の領域（横、縦）入力
	cout << "水田の大きさを入力してください(m)単位" << endl;
	cout << "横 : ";
	cin >> src_img_cols;
	cout << "縦 : ";
	cin >> src_img_rows;

	src_img_cols *= 100;
	src_img_rows *= 100;

	if (src_img_cols < 300 || src_img_rows < 300) //@comment 3x3(m)以上の領域を指定
	{
		cout << endl;
		cout << "※ 縦、横それぞれ３ｍ以上を指定してください" << endl;
		cout << endl;
		system("PAUSE");
		exit(0);
	}

	//@comment カメラの呼び出し pcのカメラ : 0 webカメラ : 1 (環境によって変化)
	VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640); //@comment webカメラの横幅を設定
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480); //@comment webカメラの縦幅を設定
	if (!cap.isOpened()) return -1; //@comment 呼び出しミスがあれば終了
	
	//@comment OptCam用
	nm30_init();
	nm30_set_panorama_mode(1, 11);

	//@comment 始めの方のフレームは暗い可能性があるので読み飛ばす
	for (int i = 0; i < 10; i++) {
		cap >> src_frame; //@comment 1フレーム取得
	}

	cout << "水田の４点をクリックしてください" << endl;

	//------------------座標取得-----------------------------------------------
	//@comment 画像中からマウスで4点を取得その後ESCキーを押すと変換処理が開始する
	namedWindow("getCoordinates");
	imshow("getCoordinates", src_frame);
	//@comment 変換したい四角形の四隅の座標をとる(クリック)
	cvSetMouseCallback("getCoordinates", getCoordinates, NULL);
	waitKey(0);
	destroyAllWindows();


	//------------------透視変換-----------------------------------------------
	Point2f pts1[] = { Point2f(Ax, Ay), Point2f(Bx, By),
		Point2f(Cx, Cy), Point2f(Dx, Dy) };

	Point2f pts2[] = { Point2f(0, src_img_rows), Point2f(0, 0),
		Point2f(src_img_cols, 0), Point2f(src_img_cols, src_img_rows) };

	//@comment 透視変換行列を計算
	Mat perspective_matrix = getPerspectiveTransform(pts1, pts2);
	Mat dst_img, colorExtra;

	//@comment 変換(線形補完)
	warpPerspective(src_frame, dst_img, perspective_matrix, Size(src_img_cols, src_img_rows), INTER_LINEAR);

	//@comment 変換前後の座標を描画
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


	int frame = 0; //@comment フレーム数保持変数
	Mat plot_img;
	dst_img.copyTo(plot_img);
	//target_itr = allTarget.begin();
	int color_r = 0, color_g = 0, color_b = 0;
	int color_flag = 0;


	//4.送信
	char id = A;
	char command = 's';
	int key;

	// ファイル書き込み
	ofs << src_img_cols << ", " << src_img_rows << endl;
	ofs << "x軸, y軸（補正なし）, ypos（補正あり）" << endl;

	Control control(src_img_cols, src_img_rows);//@comment インスタンス生成
	control.set_target();

	namedWindow("colorExt",1);
	createTrackbar("H","colorExt",&h_value,180);
	createTrackbar("S", "colorExt", &s_value, 255);
	createTrackbar("V", "colorExt", &v_value, 255);
	
	while (1) { //@comment 動作ループ

		cap >> src_frame;
		if (frame % 1 == 0) { //@comment　フレームの取得数を調節可能
			///////
			//2.送受信バッファ初期化
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

			//3.基本通信条件の設定
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

			//マニュアルモードに変更コマンドの送信
			//s:スタンバイ
			//m:マニュアル
			//a: オート

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
			//パケット作成・送信
			//command:シーケンス番号0～5
			else if (command >= '0' && command <= '9') {
				sentAigamoCommand(int(command - '0'));
			}

			//@comment 画像をリサイズ(大きすぎるとディスプレイに入りらないため)
			resize(src_frame, test_image2, Size(src_img_cols, src_img_rows), CV_8UC3);
			//src_frame = imp.undist(src_frame); //@comment カメラの歪みをとる(GoPro魚眼)

			//--------------------グレースケール化---------------------------------------

			//変換(線形補完)
			warpPerspective(src_frame, dst_img, perspective_matrix, Size(src_img_cols, src_img_rows), INTER_LINEAR);
			//@comment hsvを利用して赤色を抽出
			//入力画像、出力画像、変換、h最小値、h最大値、s最小値、s最大値、v最小値、v最大値 h:(0-180)実際の1/2
			imp.colorExtraction(&dst_img, &colorExtra, CV_BGR2HSV, 0, h_value, s_value, 255, v_value, 255);
			//colorExtraction(&dst_img, &colorExtra, CV_BGR2HSV, 145, 165,70, 255, 70, 255);
			colorExtra.copyTo(extra_img);
			cvtColor(colorExtra, colorExtra, CV_BGR2GRAY);//@comment グレースケールに変換


			//２値化
			//------------------しきい値目測用--------------------------------------------
			Mat binari_2;

			//----------------------二値化-----------------------------------------------
			threshold(colorExtra, binari_2, 0, 255, THRESH_BINARY);
			dilate(binari_2, binari_2, element, Point(-1, -1), 3); //膨張処理3回 最後の引数で回数を設定


			//---------------------面積計算-----------------------------------------------
			//取得した領域の中で一番面積の大きいものを対象としてその対象の重心を求める。
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
			
			//---------------------重心取得---------------------------------------------
			Point2i point;
			if (!GRAVITY)
			{
				point = imp.calculate_center(binari_2);//@comment momentで白色部分の重心を求める

				//cout << "posion: " << point.x << " " << point.y << endl;//@comment 重心点の表示
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
				ydef = src_img_rows - point.y;//@comment 補正なしｙ重心
				cout << point.x << " " << ypos << endl; //@comment 変換画像中でのロボットの座標(重心)
				ofs << point.x << ", " << ydef << ", " << ypos << endl; //@comment 変換
			}

			//---------------------ロボットの動作取得------------------------------------
			//if (frame % 2 == 0){
			P1 = { point.x, src_img_rows - ydef };
			if (P1.x != 0 && P1.y != 0) {
				//line(dst_img, P1, P0[4], Scalar(255, 0, 0), 2, CV_AA);
				// ターゲットの更新
				control.is_updateTarget();
				// 現在のロボットの位置情報の更新
				control.set_point(P1);
				// ロボットの動作決定
				action = control.robot_action(P0[4]);
				// ターゲットの訪問回数更新
				//num = control.target_count();
				
				control.heatmap(control.area_count(),heatmap_img,bar);
				// 内外判定
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

			
			//-------------------重心点のプロット----------------------------------------- 
			if (!point.y == 0) { //@comment point.y == 0の場合はexceptionが起こる( 0除算 )
				//@comment 画像，円の中心座標，半径，色(青)，線太さ，種類(-1, CV_AAは塗りつぶし)
				circle(dst_img, Point(point.x, point.y), 8, Scalar(255, 255, 255), -1, CV_AA);
				circle(dst_img, Point(point.x, point.y + 6 * ((1000 / point.y) + 1)), 8, Scalar(0, 0, 0), -1, CV_AA);


				//@comment 重心点の移動履歴
				circle(plot_img, Point(point.x, point.y ), 8, Scalar(0, 0, 255), -1, CV_AA);
				if (waitKey(30) == 114) {
					namedWindow("plot_img", 1);
					imshow("plot_img", plot_img);
				}
			}

			//------------------ターゲットのプロット--------------------------------------
			control.plot_target(dst_img,P0[4]);


			//------------------マスのプロット--------------------------------------

			for (int i = 0; i <= src_img_cols; i += 100) {
				for (int j = 0; j <= src_img_rows; j += 100) {

					line(dst_img, Point(i, j), Point(i, src_img_cols), Scalar(200, 200, 200), 3);
					line(dst_img, Point(i, j), Point(src_img_rows, j), Scalar(200, 200, 200), 3);
				}
			}

			//------------------直進領域のプロット--------------------------------------
			cv::Point2i A = { 100, src_img_rows - 100 }, B = { 100, 100 }, C = { src_img_cols - 100, 100 }, D = { src_img_cols - 100, src_img_rows - 100 };

			line(dst_img, Point(A), Point(B), Scalar(200, 0, 0), 3);
			line(dst_img, Point(B), Point(C), Scalar(200, 0, 0), 3);
			line(dst_img, Point(C), Point(D), Scalar(200, 0, 0), 3);
			line(dst_img, Point(D), Point(A), Scalar(200, 0, 0), 3);

			//---------------------表示部分----------------------------------------------

			resize(dst_img, dst_img, Size(700, 700));
			resize(extra_img, extra_img, Size(700, 700));
	
			imshow("dst_image", dst_img);//@comment 出力画像
			imshow("colorExt", extra_img);//@comment 赤抽出画像
			imshow("plot_img", plot_img);

			//@comment "q"を押したらプログラム終了
			if (src_frame.empty() || waitKey(50) == 113)
			{
				destroyAllWindows();
				return 0;
			}
		}
		frame++;
	}

	ofs.close(); //@comment ファイルストリームの解放
	CloseHandle(arduino);
	system("PAUSE");
}

