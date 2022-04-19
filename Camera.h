#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "DirectXBase.h"
#include "struct.h"

using namespace DirectX;

#define PLAYER_CAMERA_POS 800
#define PLAYER_CAMERA_POS_Y 200

class Camera :public Singleton<Camera>
{
public:
	static XMMATRIX matView;		//�r���[�s��
	static XMFLOAT3 eye;			//�Q�[�����[���h���ł̃J�������W
	static XMFLOAT3 target;			//�Q�[�����[���h���ŃJ���������Ă�����W
	static XMFLOAT3 up;				//�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
	static XMMATRIX matPerspective; //�ˉe�ϊ��s��
	static XMMATRIX matProjection;	//���s���e�s��

	//�R���X�g���N�^
	Camera();
	friend Singleton<Camera>;

	//������
	static void Init(Sprite& player);
	//�r���[�s�񐶐�
	static void GenerateMatView();

	static void FollowingPlayer(Sprite& player, const XMFLOAT3& forwardVec);
};