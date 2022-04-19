#include"SceneManager.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//DirectX初期化
	DirectXBase DxBase;
	DxBase.Init();

	SceneManager::Instance()->Setting();

	srand(time(NULL));

	while (true) //ゲームループ
	{
		SceneManager::Instance()->Update();

		//描画前処理
		DxBase.BeforeDrawing();

		SceneManager::Instance()->Draw();
		//描画後処理
		DxBase.AfterDrawing();
	}

	return 0;
}