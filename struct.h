#pragma once
#include"DirectXBase.h"
#include<DirectXTex.h>
#include<xaudio2.h>
#include<timeapi.h>
#include<stdlib.h>

#pragma comment(lib,"xaudio2.lib")

using namespace DirectX;

//頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos;		//xyz座標
	XMFLOAT3 normal;	//法線ベクトル	
	XMFLOAT2 uv;		//uv座標
};

//シェーダー側に渡すための基本的な行列データ
struct MatrixData
{
	XMMATRIX world;			//回転させたり移動させたりする行列
	XMMATRIX viewproj;		//ビューとプロジェクション合成行列
};

//定数バッファ用データ構造体
struct ConstBufferData
{
	XMFLOAT4 color;		//RGBA
	MatrixData mat;		//3D変換行列
};

//スプライト用データ構造体
struct Sprite
{
	bool isDisplay;								//表示用フラグ
	int textureIDnum;							//テクスチャの識別番号
	int projectionID;							//オブジェクト別に投影を使い分けるためのID
	vector<Vertex> vertices;					//頂点
	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
	vector<unsigned short> indices;				//インデックス
	ComPtr<ID3D12Resource> indexBuff;			//インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView;				//インデックスバッファビュー
	ConstBufferData constBufferData;			//定数
	ComPtr<ID3D12Resource> constBuff;			//定数バッファ
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;	//定数バッファビュー
	XMMATRIX matScale;							//スケーリング行列
	XMMATRIX matTrans;							//平行移動行列
	XMMATRIX matRot;							//回転行列
	XMFLOAT3 pos;								//座標
	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//定数バッファビューデスクリプタヒープ
	float radius;								//半径
	XMFLOAT2 anchorPoint;						//アンカーポイント
	bool isFlipX;								//左右反転用フラグ
	bool isFlipY;								//上下反転用フラグ
	XMFLOAT4 SavePos;							//上下左右の情報を保存するための変数
};

struct Texture
{
	LPCWSTR fileName;					//ファイル名
	int IDnum;							//識別番号
	ComPtr<ID3D12Resource> texBuff;		//テクスチャバッファ
	TexMetadata metadata;
	ScratchImage* scratchImg;
	Image* image;
	XMFLOAT4 colorData;
};

enum projectionID
{
	backGround,		//平行投影
	object,			//透視投影
	billBoard
};

struct ObjectData
{
	int dataID;							//図形識別ID
	vector<unsigned short> indices;		//インデックス
	vector<Vertex> vertices;			//頂点	
};

enum ObjectDataID
{
	Object,
	Cone,
	Cylinder,
	Sphere
};

//レイ構造体
struct Ray {
	XMVECTOR pos;
	XMVECTOR dir;
};
//レイトの当たり判定用のポリゴン構造体
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

//サウンド関係の構造体
//チャンクヘッダ
struct ChunkHeader {
	char id[4];		//チャンク毎のID
	int32_t size;	//チャンクサイズ
};
//RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	//"RIFF"
	char type[4];	//"WAVE
};
//FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;		//"fmt"
	WAVEFORMATEX fmt;	//波形フォーマット
};

//音声データ
struct SoundData {
	WAVEFORMATEX wfex;			//波形フォーマット
	BYTE* pBuffer;				//バッファの先頭アドレス
	unsigned int bufferSize;	//バッファのサイズ
	IXAudio2SourceVoice* source;
	float volume;
};