#pragma once
#include"struct.h"

class TextureManager :public Singleton<TextureManager>
{
	static ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	static vector<Texture> texture;

	//�R���X�g���N�^
	TextureManager();
	friend Singleton<TextureManager>;

public:
	//�e�N�X�`�������[�h
	int LoadTexture(LPCWSTR fileName);
	//�e�N�X�`���𐶐�
	int CreateTexture(XMFLOAT4 color);
	//���ʔԍ��̏��Ԃ̃V�F�[�_�[���\�[�X�r���[��Ԃ��B
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(int IDnum);
	//�f�B�X�N���v�^�q�[�v��Ԃ��B
	ComPtr<ID3D12DescriptorHeap> GetDescHeap();
};