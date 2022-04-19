#pragma once
#include "TextureManager.h"
#include "ObjectManager.h"
#include <d3dcompiler.h>
#include"Camera.h"

#pragma comment(lib,"d3dcompiler.lib")

const int ConeVertexNum = 30;

class SpriteManager :public Singleton<SpriteManager>
{
public:
	ComPtr<ID3D12PipelineState> pipelinestate;
	ComPtr<ID3D12RootSignature> rootsignature;
	//�R���X�g���N�^
	SpriteManager();
	friend Singleton<SpriteManager>;

	/*------------������-------------*/

	//��������
	Sprite InitLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, XMFLOAT4 color);

	//2D������
	Sprite Init2DTexture(XMFLOAT3 pos, XMFLOAT2 size, int projectionID, LPCWSTR fileName);
	
	//�����̏�����
	Sprite InitBox(XMFLOAT3 pos, XMFLOAT3 size, int projectionID, LPCWSTR fileName);

	//���p��������
	Sprite InitCone(XMFLOAT3 pos, int projectionID, XMFLOAT4 color, LPCWSTR fileName);

	//�~���̏���������
	Sprite InitCylinder(XMFLOAT3 pos, float radius, float height, int projectionID, XMFLOAT4 color, LPCWSTR fileName = 0);

	//���̏���������
	Sprite InitSphere(XMFLOAT3 pos, float radius, int projectionID, XMFLOAT4 color, LPCWSTR fileName = 0);
	
	Sprite Init3DObject(XMFLOAT3 pos, int projectionID, LPCWSTR texFileName, const char* objFileName);

	//���ʂ̏���������
	void CommonInit(int blend, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType);

	/*-------------------------------*/

	/*-------------�`��--------------*/

	//���ʂ̕`�揈��
	void CommonDraw(D3D12_PRIMITIVE_TOPOLOGY topology);

	//�X�v���C�g�ʂ̕`�揈��
	void Draw(Sprite& sprite);

	/*-------------------------------*/

	/*-----------�s��ϊ�------------*/
	
	//�X�P�[�����O
	void ChangeScale(Sprite& sprite, XMFLOAT3 scale);

	//��]
	void ChangeRotation(Sprite& sprite, XMFLOAT3 rotation);
	
	//���s�ړ�(���)
	void ChangePosition(Sprite& sprite, XMFLOAT3 position);
	
	//���s�ړ�(���Z)
	void ChangePositionAdd(Sprite& sprite, XMFLOAT3 position);

	/*-------------------------------*/

	/*--------------���]-------------*/

	//���]
	void Invertion(Sprite& sprite, bool isFlipX, bool isFlipY);

	/*-------------------------------*/
};