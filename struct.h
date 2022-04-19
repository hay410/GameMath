#pragma once
#include"DirectXBase.h"
#include<DirectXTex.h>
#include<xaudio2.h>
#include<timeapi.h>
#include<stdlib.h>

#pragma comment(lib,"xaudio2.lib")

using namespace DirectX;

//���_�f�[�^�\����
struct Vertex
{
	XMFLOAT3 pos;		//xyz���W
	XMFLOAT3 normal;	//�@���x�N�g��	
	XMFLOAT2 uv;		//uv���W
};

//�V�F�[�_�[���ɓn�����߂̊�{�I�ȍs��f�[�^
struct MatrixData
{
	XMMATRIX world;			//��]��������ړ��������肷��s��
	XMMATRIX viewproj;		//�r���[�ƃv���W�F�N�V���������s��
};

//�萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferData
{
	XMFLOAT4 color;		//RGBA
	MatrixData mat;		//3D�ϊ��s��
};

//�X�v���C�g�p�f�[�^�\����
struct Sprite
{
	bool isDisplay;								//�\���p�t���O
	int textureIDnum;							//�e�N�X�`���̎��ʔԍ�
	int projectionID;							//�I�u�W�F�N�g�ʂɓ��e���g�������邽�߂�ID
	vector<Vertex> vertices;					//���_
	ComPtr<ID3D12Resource> vertBuff;			//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView;			//���_�o�b�t�@�r���[
	vector<unsigned short> indices;				//�C���f�b�N�X
	ComPtr<ID3D12Resource> indexBuff;			//�C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView;				//�C���f�b�N�X�o�b�t�@�r���[
	ConstBufferData constBufferData;			//�萔
	ComPtr<ID3D12Resource> constBuff;			//�萔�o�b�t�@
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;	//�萔�o�b�t�@�r���[
	XMMATRIX matScale;							//�X�P�[�����O�s��
	XMMATRIX matTrans;							//���s�ړ��s��
	XMMATRIX matRot;							//��]�s��
	XMFLOAT3 pos;								//���W
	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//�萔�o�b�t�@�r���[�f�X�N���v�^�q�[�v
	float radius;								//���a
	XMFLOAT2 anchorPoint;						//�A���J�[�|�C���g
	bool isFlipX;								//���E���]�p�t���O
	bool isFlipY;								//�㉺���]�p�t���O
	XMFLOAT4 SavePos;							//�㉺���E�̏���ۑ����邽�߂̕ϐ�
};

struct Texture
{
	LPCWSTR fileName;					//�t�@�C����
	int IDnum;							//���ʔԍ�
	ComPtr<ID3D12Resource> texBuff;		//�e�N�X�`���o�b�t�@
	TexMetadata metadata;
	ScratchImage* scratchImg;
	Image* image;
	XMFLOAT4 colorData;
};

enum projectionID
{
	backGround,		//���s���e
	object,			//�������e
	billBoard
};

struct ObjectData
{
	int dataID;							//�}�`����ID
	vector<unsigned short> indices;		//�C���f�b�N�X
	vector<Vertex> vertices;			//���_	
};

enum ObjectDataID
{
	Object,
	Cone,
	Cylinder,
	Sphere
};

//���C�\����
struct Ray {
	XMVECTOR pos;
	XMVECTOR dir;
};
//���C�g�̓����蔻��p�̃|���S���\����
struct Porygon {
	bool isActive = true;
	Vertex p1;
	Vertex p2;
	Vertex p3;
};


enum BlendID
{
	alphablend,
	addblend
};

//�T�E���h�֌W�̍\����
//�`�����N�w�b�_
struct ChunkHeader {
	char id[4];		//�`�����N����ID
	int32_t size;	//�`�����N�T�C�Y
};
//RIFF�w�b�_�`�����N
struct RiffHeader {
	ChunkHeader chunk;	//"RIFF"
	char type[4];	//"WAVE
};
//FMT�`�����N
struct FormatChunk {
	ChunkHeader chunk;		//"fmt"
	WAVEFORMATEX fmt;	//�g�`�t�H�[�}�b�g
};

//�����f�[�^
struct SoundData {
	WAVEFORMATEX wfex;			//�g�`�t�H�[�}�b�g
	BYTE* pBuffer;				//�o�b�t�@�̐擪�A�h���X
	unsigned int bufferSize;	//�o�b�t�@�̃T�C�Y
	IXAudio2SourceVoice* source;
	float volume;
};