#include"SceneManager.h"

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//DirectX������
	DirectXBase DxBase;
	DxBase.Init();

	SceneManager::Instance()->Setting();

	srand(time(NULL));

	while (true) //�Q�[�����[�v
	{
		SceneManager::Instance()->Update();

		//�`��O����
		DxBase.BeforeDrawing();

		SceneManager::Instance()->Draw();
		//�`��㏈��
		DxBase.AfterDrawing();
	}

	return 0;
}