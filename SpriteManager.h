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
	//コンストラクタ
	SpriteManager();
	friend Singleton<SpriteManager>;

	/*------------初期化-------------*/

	//線初期化
	Sprite InitLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, XMFLOAT4 color);

	//2D初期化
	Sprite Init2DTexture(XMFLOAT3 pos, XMFLOAT2 size, int projectionID, LPCWSTR fileName);
	
	//立方体初期化
	Sprite InitBox(XMFLOAT3 pos, XMFLOAT3 size, int projectionID, LPCWSTR fileName);

	//○角錐初期化
	Sprite InitCone(XMFLOAT3 pos, int projectionID, XMFLOAT4 color, LPCWSTR fileName);

	//円柱の初期化処理
	Sprite InitCylinder(XMFLOAT3 pos, float radius, float height, int projectionID, XMFLOAT4 color, LPCWSTR fileName = 0);

	//球の初期化処理
	Sprite InitSphere(XMFLOAT3 pos, float radius, int projectionID, XMFLOAT4 color, LPCWSTR fileName = 0);
	
	Sprite Init3DObject(XMFLOAT3 pos, int projectionID, LPCWSTR texFileName, const char* objFileName);

	//共通の初期化処理
	void CommonInit(int blend, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType);

	/*-------------------------------*/

	/*-------------描画--------------*/

	//共通の描画処理
	void CommonDraw(D3D12_PRIMITIVE_TOPOLOGY topology);

	//スプライト別の描画処理
	void Draw(Sprite& sprite);

	/*-------------------------------*/

	/*-----------行列変換------------*/
	
	//スケーリング
	void ChangeScale(Sprite& sprite, XMFLOAT3 scale);

	//回転
	void ChangeRotation(Sprite& sprite, XMFLOAT3 rotation);
	
	//平行移動(代入)
	void ChangePosition(Sprite& sprite, XMFLOAT3 position);
	
	//平行移動(加算)
	void ChangePositionAdd(Sprite& sprite, XMFLOAT3 position);

	/*-------------------------------*/

	/*--------------反転-------------*/

	//反転
	void Invertion(Sprite& sprite, bool isFlipX, bool isFlipY);

	/*-------------------------------*/
};