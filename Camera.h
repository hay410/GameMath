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
	static XMMATRIX matView;		//ビュー行列
	static XMFLOAT3 eye;			//ゲームワールド内でのカメラ座標
	static XMFLOAT3 target;			//ゲームワールド内でカメラが見ている座標
	static XMFLOAT3 up;				//ゲームワールド内でカメラから見て上方向を指すベクトル
	static XMMATRIX matPerspective; //射影変換行列
	static XMMATRIX matProjection;	//平行投影行列

	//コンストラクタ
	Camera();
	friend Singleton<Camera>;

	//初期化
	static void Init(Sprite& player);
	//ビュー行列生成
	static void GenerateMatView();

	static void FollowingPlayer(Sprite& player, const XMFLOAT3& forwardVec);
};