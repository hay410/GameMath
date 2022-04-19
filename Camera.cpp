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
	eye = XMFLOAT3(10, 0, 20);		//���_���W		�Q�[�����[���h���ł̃J�������W
	target = XMFLOAT3(10, 0, 0);		//�����_���W		�Q�[�����[���h���ŃJ���������Ă�����W
	up = XMFLOAT3(0, -1, 0);										//������x�N�g��	�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
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
	//��x�N�g�����v�Z����
	XMFLOAT3 upperVec = { 0,-1,0 };
	XMVECTOR a = XMVector3Transform(XMLoadFloat3(&upperVec), player.matRot);
	XMStoreFloat3(&upperVec, a);

	//�J�����̍��W�𓮂���
	eye = XMFLOAT3(player.pos.x - forwardVec.x * PLAYER_CAMERA_POS + upperVec.x * PLAYER_CAMERA_POS_Y, player.pos.y - forwardVec.y * PLAYER_CAMERA_POS + upperVec.y * PLAYER_CAMERA_POS_Y, player.pos.z - forwardVec.z * PLAYER_CAMERA_POS + upperVec.z * PLAYER_CAMERA_POS_Y);
	//�J�����̒����_���W�𓮂���
	target = XMFLOAT3(player.pos.x + forwardVec.x * PLAYER_CAMERA_POS, player.pos.y + forwardVec.y * PLAYER_CAMERA_POS, player.pos.z + forwardVec.z * PLAYER_CAMERA_POS);
	//��x�N�g�����X�V����
	up = upperVec;
}
