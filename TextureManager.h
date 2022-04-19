#pragma once
#include"struct.h"

class TextureManager :public Singleton<TextureManager>
{
	static ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	static vector<Texture> texture;

	//コンストラクタ
	TextureManager();
	friend Singleton<TextureManager>;

public:
	//テクスチャをロード
	int LoadTexture(LPCWSTR fileName);
	//テクスチャを生成
	int CreateTexture(XMFLOAT4 color);
	//識別番号の順番のシェーダーリソースビューを返す。
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(int IDnum);
	//ディスクリプタヒープを返す。
	ComPtr<ID3D12DescriptorHeap> GetDescHeap();
};