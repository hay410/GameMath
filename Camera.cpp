#include"Camera.h"

XMFLOAT3 Camera::eye = XMFLOAT3(WIN_WIDTH / 2, WIN_HEIGHT / 2, 500);
XMFLOAT3 Camera::target = XMFLOAT3(WIN_WIDTH / 2, WIN_HEIGHT / 2, 0);
XMFLOAT3 Camera::up = XMFLOAT3(0, -1, 0);
XMMATRIX Camera::matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
XMMATRIX Camera::matPerspective = XMMatrixPerspectiveFovLH(
	XMConvertToRadians(60.0f),
	(float)WIN_WIDTH / WIN_HEIGHT,
	0.1f, 3000.0f
);
XMMATRIX Camera::matProjection = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, WIN_HEIGHT, 0, 1.0f, 1000.0f);


Camera::Camera() {
	eye = XMFLOAT3(10, 0, 20);		//視点座標		ゲームワールド内でのカメラ座標
	target = XMFLOAT3(10, 0, 0);		//注視点座標		ゲームワールド内でカメラが見ている座標
	up = XMFLOAT3(0, -1, 0);										//上方向ベクトル	ゲームワールド内でカメラから見て上方向を指すベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		(float)WIN_WIDTH / WIN_HEIGHT,
		0.1f,20000.0f
	);
	matProjection = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, WIN_HEIGHT, 0, 0.1f, 20000.0f);

}

void Camera::Init(Sprite& player)
{
}

void Camera::GenerateMatView()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		(float)WIN_WIDTH / WIN_HEIGHT,
		0.1f, 20000.0f
	);
	matProjection = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, WIN_HEIGHT, 0, 0.1f, 20000.0f);
}

void Camera::FollowingPlayer(Sprite& player, const XMFLOAT3& forwardVec)
{
	//上ベクトルを計算する
	XMFLOAT3 upperVec = { 0,-1,0 };
	XMVECTOR a = XMVector3Transform(XMLoadFloat3(&upperVec), player.matRot);
	XMStoreFloat3(&upperVec, a);

	//カメラの座標を動かす
	eye = XMFLOAT3(player.pos.x - forwardVec.x * PLAYER_CAMERA_POS + upperVec.x * PLAYER_CAMERA_POS_Y, player.pos.y - forwardVec.y * PLAYER_CAMERA_POS + upperVec.y * PLAYER_CAMERA_POS_Y, player.pos.z - forwardVec.z * PLAYER_CAMERA_POS + upperVec.z * PLAYER_CAMERA_POS_Y);
	//カメラの注視点座標を動かす
	target = XMFLOAT3(player.pos.x + forwardVec.x * PLAYER_CAMERA_POS, player.pos.y + forwardVec.y * PLAYER_CAMERA_POS, player.pos.z + forwardVec.z * PLAYER_CAMERA_POS);
	//上ベクトルを更新する
	up = upperVec;
}
