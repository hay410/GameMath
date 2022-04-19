#include"WindowsAPI.h"


LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY: //ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); //標準の処理を行う
}

void WindowsAPI::Init()
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; //ウィンドウプロシ―ジャを設定
	w.lpszClassName = L"DirectXGame"; //ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr); //ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //カーソル指定

	//ウィンドウクラスをOSに登録
	RegisterClassEx(&w);

	//ウィンドウサイズ{ x座標 y座標 横軸 縦軸 }
	wrc = { 0,0,width,height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); //自動でサイズ補正

	//ウィンドウオブジェクトの生成
		hwnd = CreateWindow(w.lpszClassName, //クラス名
		L"評価課題②",						  //タイトルバーの文字
		WS_OVERLAPPEDWINDOW,				  //標準なウィンドウサイズ
		CW_USEDEFAULT,						  //表示X座標(OSに任せる)
		CW_USEDEFAULT,						  //表示Y座標(OSに任せる)
		wrc.right - wrc.left,				  //ウィンドウ横軸
		wrc.bottom - wrc.top,				  //ウィンドウ縦軸
		nullptr,							  //親ウィンドウハンドル
		nullptr,							  //メニューハンドル
		w.hInstance,						  //呼び出しアプリケーションハンドル
		nullptr);							  //オプション

			//ウィンドウ表示
		ShowWindow(hwnd, SW_SHOW);
}