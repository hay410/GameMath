#pragma once
#include"SpriteManager.h"

class SceneManager :public Singleton<SceneManager>
{
public:
	/*---- メンバ定数 ----*/


	enum SceneNum {
		TITLE_SCENE,				//タイトル画面
		GAME_SCENE,				//ゲーム画面
		END_SCENE				//エンド画面
	};

	const int FPS = 60;

	/*---- メンバ変数 ----*/
	
	int sceneNum;

	float gravity;

	Sprite sphere;

	/*---- メンバ関数 ----*/
	//コンストラクタ
	SceneManager();
	friend Singleton<SceneManager>;

	void Setting();

	//初期化
	void Init();

	//更新
	void Update();

	//描画
	void Draw();


	//titleSceneでの更新
	void UpdateTitleScene();

	//titleSceneでの描画
	void DrawTitleScene();

	//gameSceneでの更新
	void UpdateGameScene();

	//gameSceneでの描画
	void DrawGameScene();

	//endSceneでの更新
	void UpdateEndScene();

	//endSceneでの描画
	void DrawEndScene();


};