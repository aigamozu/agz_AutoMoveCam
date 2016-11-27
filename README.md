# agz_AutoMoveCam


##OpenCVの使い方
###utilsブランチからopencvを任意の場所へコピー
* visual studio上の [表示] ⇒ [その他のウィンドウ] ⇒ [プロパティマネージャー] を開く
* [Debug|Win32] をクリックし, [C/C++] ⇒ [全般] ⇒ [追加のインクルードディレクトリ] に "opencv/build/include" までのpathを記入
* 続いて, [リンカー] ⇒ [全般] ⇒ [追加のライブラリディレクトリ] に "opencv/x86/vc12/lib" までのpathを記入
* 最後に, [リンカー] ⇒ [入力] ⇒ [追加の依存ファイル] に"opencv_core249d.lib","opencv_highgui249d.lib","opencv_imgproc249d.lib"を記入

##OptCamSDKの使い方
###utilsブランチからOptCamSDKを任意の場所へコピー
* OpenCVの使い方を参考に[プロパティマネージャー]を開く
* [Debug|Win32] をクリックし, [C/C++] ⇒ [全般] ⇒ [追加のインクルードディレクトリ] に "OptCamSDK" までのpathを記入
* 続いて, [リンカー] ⇒ [全般] ⇒ [追加のライブラリディレクトリ] に "OptCamSDK" までのpathを記入
* 最後に, [リンカー] ⇒ [入力] ⇒ [追加の依存ファイル] に"SSK.lib"を記入

