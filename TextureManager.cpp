#include"TextureManager.h"

ComPtr<ID3D12DescriptorHeap> TextureManager::srvDescHeap = nullptr;
vector<Texture> TextureManager::texture = {};

TextureManager::TextureManager(){
	//ディスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;		//シェーダーから見える
	descHeapDesc.NumDescriptors = 256;									//SRV一つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&srvDescHeap));
}

int TextureManager::LoadTexture(LPCWSTR fileName)
{
	//ファイルがロード済みかチェック
	if (texture.size() > 0)
	{
		for (int i = 0; i < texture.size(); ++i) {
			int a = texture.size();
			//ロードしてあったら識別番号を返す
			if (texture.at(i).fileName == fileName) {
				return texture.at(i).IDnum;
			}
		}
	}

	//ロードしてなかったらロードする
	TexMetadata metadata;
	ScratchImage scratchImg;
	HRESULT result = LoadFromWICFile(
		fileName,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	const Image* img = scratchImg.GetImage(0, 0, 0);

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テクスチャバッファの生成
	ComPtr<ID3D12Resource> texbuff = nullptr;
	result = DirectXBase::Instance()->dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff)
	);

	//データ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,		//全領域へコピー
		img->pixels,		//元データアドレス
		(UINT)img->rowPitch,		//1ラインサイズ
		(UINT)img->slicePitch		//全サイズ
	);

	//メンバのテクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	Texture tex;
	tex.fileName = fileName;
	tex.IDnum = texture.size();
	tex.metadata = metadata;
	tex.scratchImg = &scratchImg;
	tex.texBuff = texbuff;
	texture.push_back(tex);

	//ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		texture.size()-1,
		DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	);
	
	//シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	
	//ヒープにシェーダーリソースビュー生成
	DirectXBase::Instance()->dev->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	return texture.at(texture.size()-1).IDnum;
}

int TextureManager::CreateTexture(XMFLOAT4 color)
{
	//同じ色のテクスチャがすでに生成済みかをチェックする
	for (int i = 0; i < texture.size(); ++i) {
		if (texture.at(i).fileName == L"selfTexture" && texture.at(i).colorData.x == color.x && texture.at(i).colorData.y == color.y &&
			texture.at(i).colorData.z == color.z && texture.at(i).colorData.w == color.w) {
			//すでに生成してあるテクスチャなのでSRVヒープの番号を返す
			return i;
		}
	}

	//画像データを作成する
	const int texWidth = 256;
	const int imageDataCount = texWidth * texWidth;
	//画像イメージデータ配列
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//全ピクセルの色を初期化
	for (int i = 0; i < imageDataCount; ++i) {
		imageData[i].x = color.x;
		imageData[i].y = color.y;
		imageData[i].z = color.z;
		imageData[i].w = color.w;
	}

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		texWidth,
		(UINT)texWidth,
		(UINT16)1,
		(UINT16)1);

	//テクスチャバッファの生成
	ComPtr<ID3D12Resource> texbuff = nullptr;
	HRESULT result = DirectXBase::Instance()->dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	//データ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,							//全領域コピー
		imageData,							//元データの先頭アドレス
		sizeof(XMFLOAT4) * texWidth,		//一ラインのサイズ
		sizeof(XMFLOAT4) * imageDataCount	//いちまいのサイズ
	);

	//テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	Texture proTexture{};
	proTexture.fileName = L"selfTexture";
	proTexture.IDnum = texture.size();
	proTexture.metadata = {};
	proTexture.scratchImg = {};
	proTexture.texBuff = texbuff;
	proTexture.colorData = color;
	texture.push_back(proTexture);

	//ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		srvDescHeap->GetCPUDescriptorHandleForHeapStart(),texture.size()-1, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	//ヒープにシェーダーリソースビュー生成
	DirectXBase::Instance()->dev->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	delete[] imageData;

	return texture.at(texture.size() - 1).IDnum;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRV(int IDnum)
{
	D3D12_GPU_DESCRIPTOR_HANDLE basicHeapHandle = srvDescHeap->GetGPUDescriptorHandleForHeapStart();
	//消費した分だけアドレスをずらす
	for (int i = 0; i < IDnum; ++i) {
		basicHeapHandle.ptr += DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	return basicHeapHandle;
}

ComPtr<ID3D12DescriptorHeap> TextureManager::GetDescHeap()
{
	return srvDescHeap;
}
