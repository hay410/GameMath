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
	//�r���[�s�񐶐�
	Camera::Instance()->GenerateMatView();

	switch (sceneNum) {
	case TITLE_SCENE:
		//�^�C�g����ʂł̍X�V����
 		UpdateTitleScene();
		break;
	case GAME_SCENE:
		//�Q�[���v���C���ł̍X�V����
		UpdateGameScene();
		break;
	case END_SCENE:
		//�G���h��ʂł̍X�V����
		UpdateEndScene();
		break;
	}
}

void SceneManager::Draw()
{
	SpriteManager::Instance()->CommonDraw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	switch (sceneNum) {
	case TITLE_SCENE:
		//�^�C�g����ʂł̍X�V����
		DrawTitleScene();
		break;
	case GAME_SCENE:
		//�Q�[���v���C���ł̍X�V����
		DrawGameScene();
		break;
	case END_SCENE:
		//�G���h��ʂł̍X�V����
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
	//�X�y�[�X�L�[�Ń��Z�b�g
	if (Input::isKeyTrigger(DIK_SPACE))
	{
		SpriteManager::Instance()->ChangePosition(sphere, { 0,0,0 });
		gravity = -0.3;
	}

	//�E�ɓ��������^��
	SpriteManager::Instance()->ChangePositionAdd(sphere, { 0.3,0,0 });

	//�d�͂����Z
	SpriteManager::Instance()->ChangePositionAdd(sphere, { 0,gravity,0 });

	//�d�͌v�Z
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
