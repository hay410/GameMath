#include"SceneManager.h"

SceneManager::SceneManager()
{
}

void SceneManager::Setting()
{
	sceneNum = GAME_SCENE;
	sphere = SpriteManager::Instance()->InitSphere({ 0,0,0 }, 16, object, { 1,1,1,1 });
	gravity = -0.3;
	SpriteManager::Instance()->CommonInit(alphablend, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}

void SceneManager::Init()
{
	SpriteManager::Instance()->ChangePosition(sphere, { 0,0,0 });
}

void SceneManager::Update()
{
	Input::Update(DirectXBase::Instance()->devkeybord, DirectXBase::Instance()->devmouse);
	//ビュー行列生成
	Camera::Instance()->GenerateMatView();

	switch (sceneNum) {
	case TITLE_SCENE:
		//タイトル画面での更新処理
 		UpdateTitleScene();
		break;
	case GAME_SCENE:
		//ゲームプレイ中での更新処理
		UpdateGameScene();
		break;
	case END_SCENE:
		//エンド画面での更新処理
		UpdateEndScene();
		break;
	}
}

void SceneManager::Draw()
{
	SpriteManager::Instance()->CommonDraw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	switch (sceneNum) {
	case TITLE_SCENE:
		//タイトル画面での更新処理
		DrawTitleScene();
		break;
	case GAME_SCENE:
		//ゲームプレイ中での更新処理
		DrawGameScene();
		break;
	case END_SCENE:
		//エンド画面での更新処理
		DrawEndScene();
		break;
	}
}

void SceneManager::UpdateTitleScene()
{
}

void SceneManager::DrawTitleScene()
{
}

void SceneManager::UpdateGameScene()
{
	//スペースキーでリセット
	if (Input::isKeyTrigger(DIK_SPACE))
	{
		SpriteManager::Instance()->ChangePosition(sphere, { 0,0,0 });
		gravity = -0.3;
	}

	//右に等速直線運動
	SpriteManager::Instance()->ChangePositionAdd(sphere, { 0.3,0,0 });

	//重力を加算
	SpriteManager::Instance()->ChangePositionAdd(sphere, { 0,gravity,0 });

	//重力計算
	float addGravity = 0.01f;
	gravity += addGravity;
}

void SceneManager::DrawGameScene()
{
	SpriteManager::Instance()->Draw(sphere);
}

void SceneManager::UpdateEndScene()
{
}

void SceneManager::DrawEndScene()
{
}
