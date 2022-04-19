#include"SpriteManager.h"

SpriteManager::SpriteManager(){};

Sprite SpriteManager::InitLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, XMFLOAT4 color)
{

	Sprite sprite;
	sprite.projectionID = projectionID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャを作成
	sprite.textureIDnum = TextureManager::Instance()->CreateTexture(color);

	//頂点バッファの生成
	Vertex vertex;
	vertex.pos = startPos;	//視点
	vertex.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertex);
	vertex.pos = endPos;	//終点
	vertex.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertex);

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点インデックスの設定
	sprite.indices.push_back(0);
	sprite.indices.push_back(1);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);

	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(2.0f, 2.0f, 1.0f);
	sprite.matTrans = XMMatrixIdentity();
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

Sprite SpriteManager::Init2DTexture(XMFLOAT3 pos, XMFLOAT2 size, int projectionID, LPCWSTR fileName)
{
	Sprite sprite;
	sprite.projectionID = projectionID;
	sprite.isDisplay = true;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャをロード
	sprite.textureIDnum = TextureManager::Instance()->LoadTexture(fileName);

	////頂点バッファの生成
	//float left = (0.0f - sprite.anchorPoint.x) * size.x;
	//float right = (1.0f - sprite.anchorPoint.x) * size.x;
	//float top = (0.0f - sprite.anchorPoint.y) * size.y;
	//float bottom = (1.0f - sprite.anchorPoint.y) * size.y;

	////情報を保存
	//sprite.SavePos = { left, right, top, bottom };

	//頂点バッファの生成
	Vertex vertex;
	vertex.pos = XMFLOAT3(-size.x, -size.y, 0);	//左下
	vertex.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertex);
	vertex.pos = XMFLOAT3(-size.x, size.y, 0);	//左上
	vertex.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertex);
	vertex.pos = XMFLOAT3(size.x, -size.y, 0);	//右下
	vertex.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertex);
	vertex.pos = XMFLOAT3(size.x, size.y, 0);		//右上
	vertex.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertex);


	//インデックスの設定
	sprite.indices.push_back(0);
	sprite.indices.push_back(1);
	sprite.indices.push_back(2);
	sprite.indices.push_back(2);
	sprite.indices.push_back(1);
	sprite.indices.push_back(3);

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);

	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(2.0f, 2.0f, 1.0f);
	sprite.matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

Sprite SpriteManager::InitBox(XMFLOAT3 pos, XMFLOAT3 size, int projectionID, LPCWSTR fileName)
{
	Sprite sprite;
	sprite.projectionID = projectionID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャをロード
	sprite.textureIDnum = TextureManager::Instance()->LoadTexture(fileName);

	//頂点バッファの生成
#pragma region 頂点初期化
	Vertex vertices;
	//前
	vertices.pos = XMFLOAT3(-size.x, -size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertices);	//左下
	vertices.pos = XMFLOAT3(-size.x, size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertices);	//左上
	vertices.pos = XMFLOAT3(size.x, -size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertices);	//右下
	vertices.pos = XMFLOAT3(size.x, size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertices);	//右上
	//後ろ
	vertices.pos = XMFLOAT3(size.x, -size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertices);	//左下
	vertices.pos = XMFLOAT3(size.x, size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertices);	//左上
	vertices.pos = XMFLOAT3(-size.x, -size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertices);	//右下
	vertices.pos = XMFLOAT3(-size.x, size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertices);	//右上
	//右
	vertices.pos = XMFLOAT3(size.x, -size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertices);	//左下
	vertices.pos = XMFLOAT3(size.x, size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertices);	//左上
	vertices.pos = XMFLOAT3(size.x, -size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertices);	//右下
	vertices.pos = XMFLOAT3(size.x, size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertices);	//右上
	//左
	vertices.pos = XMFLOAT3(-size.x, -size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertices);	//左下
	vertices.pos = XMFLOAT3(-size.x, size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertices);	//左上
	vertices.pos = XMFLOAT3(-size.x, -size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertices);	//右下
	vertices.pos = XMFLOAT3(-size.x, size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertices);	//右上
	//下
	vertices.pos = XMFLOAT3(-size.x, -size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertices);	//左下
	vertices.pos = XMFLOAT3(-size.x, -size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertices);	//左上
	vertices.pos = XMFLOAT3(size.x, -size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertices);	//右下
	vertices.pos = XMFLOAT3(size.x, -size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertices);	//右上
	//上
	vertices.pos = XMFLOAT3(-size.x, size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 0);
	sprite.vertices.push_back(vertices);	//左下
	vertices.pos = XMFLOAT3(-size.x, size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(0, 1);
	sprite.vertices.push_back(vertices);	//左上
	vertices.pos = XMFLOAT3(size.x, size.y, -size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 0);
	sprite.vertices.push_back(vertices);	//右下
	vertices.pos = XMFLOAT3(size.x, size.y, size.z);
	vertices.normal = XMFLOAT3(0, 0, 0);
	vertices.uv = XMFLOAT2(1, 1);
	sprite.vertices.push_back(vertices);	//右上
#pragma endregion

	//インデックスの設定
	int buff = 0;
	for (int i = 0; i < 6; ++i) {
		sprite.indices.push_back(0 + buff);
		sprite.indices.push_back(1 + buff);
		sprite.indices.push_back(2 + buff);
		sprite.indices.push_back(2 + buff);
		sprite.indices.push_back(1 + buff);
		sprite.indices.push_back(3 + buff);
		buff += 4;
	}

	//法線ベクトルの計算
	for (int i = 0; i < sprite.indices.size() / 3; ++i)
	{
		//三角形ひとつひとつ計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = sprite.indices.at(i * 3 + 0);
		unsigned short index1 = sprite.indices.at(i * 3 + 1);
		unsigned short index2 = sprite.indices.at(i * 3 + 2);
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&sprite.vertices.at(index0).pos);
		XMVECTOR p1 = XMLoadFloat3(&sprite.vertices.at(index1).pos);
		XMVECTOR p2 = XMLoadFloat3(&sprite.vertices.at(index2).pos);
		//p0->p1ベクトル,p0->p2ベクトルを計算
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外戚は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&sprite.vertices.at(index0).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index1).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index2).normal, normal);
	}

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);

	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	sprite.matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

Sprite SpriteManager::InitCone(XMFLOAT3 pos, int projectionID,XMFLOAT4 color, LPCWSTR fileName)
{
	Sprite sprite;
	sprite.projectionID = projectionID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャをロード
	if (fileName != 0)
	{
		sprite.textureIDnum = TextureManager::Instance()->LoadTexture(fileName);
	}
	else
	{
		sprite.textureIDnum = TextureManager::Instance()->CreateTexture(color);
	}

	//頂点の生成
	ObjectManager::LoadObject(Cone, sprite);

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);
	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	sprite.matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	sprite.constBufferData.color = color;
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

Sprite SpriteManager::InitCylinder(XMFLOAT3 pos, float radius, float height, int projectionID, XMFLOAT4 color, LPCWSTR fileName)
{
	Sprite sprite;
	sprite.projectionID = projectionID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャをロード
	//テクスチャをロード
	if (fileName != 0)
	{
		sprite.textureIDnum = TextureManager::Instance()->LoadTexture(fileName);
	}
	else
	{
		sprite.textureIDnum = TextureManager::Instance()->CreateTexture(color);
	}

	//頂点の生成
	ObjectManager::LoadObject(Cylinder, sprite);

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);
	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	sprite.matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	sprite.constBufferData.color = color;
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

Sprite SpriteManager::InitSphere(XMFLOAT3 pos, float radius, int projectionID, XMFLOAT4 color, LPCWSTR fileName)
{
	Sprite sprite;
	sprite.projectionID = projectionID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャをロード
	if (fileName != 0)
	{
		sprite.textureIDnum = TextureManager::Instance()->LoadTexture(fileName);
	}
	else
	{
		sprite.textureIDnum = TextureManager::Instance()->CreateTexture(color);
	}

	//頂点の生成
	ObjectManager::LoadObject(Sphere, sprite);

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);
	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	sprite.matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	//sprite.constBufferData.color = color;
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

Sprite SpriteManager::Init3DObject(XMFLOAT3 pos, int projectionID, LPCWSTR texFileName, const char* objFileName)
{
	Sprite sprite;
	sprite.projectionID = projectionID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&sprite.constDescHeap));

	//テクスチャをロード
	sprite.textureIDnum = TextureManager::Instance()->LoadTexture(texFileName);

	ObjectManager::LoadObject(Object, sprite, objFileName);

	//法線ベクトルの計算
	for (int i = 0; i < sprite.indices.size() / 3; ++i) {
		//三角形ひとつひとつ計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = sprite.indices.at(i * 3 + 0);
		unsigned short index1 = sprite.indices.at(i * 3 + 1);
		unsigned short index2 = sprite.indices.at(i * 3 + 2);
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&sprite.vertices.at(index0).pos);
		XMVECTOR p1 = XMLoadFloat3(&sprite.vertices.at(index1).pos);
		XMVECTOR p2 = XMLoadFloat3(&sprite.vertices.at(index2).pos);
		//p0->p1ベクトル,p0->p2ベクトルを計算
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外戚は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		//spriteへのコピー
		XMStoreFloat3(&sprite.vertices.at(index0).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index1).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index2).normal, normal);
	}

	//頂点バッファビューの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.vertices.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff)
	);

	//頂点バッファビューの設定
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sprite.vertices.size() * sizeof(Vertex);
	sprite.vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファの設定
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sprite.indices.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.indexBuff)
	);

	//インデックスバッファビューの設定
	sprite.ibView.BufferLocation = sprite.indexBuff->GetGPUVirtualAddress();
	sprite.ibView.Format = DXGI_FORMAT_R16_UINT;
	sprite.ibView.SizeInBytes = sprite.indices.size() * sizeof(unsigned short);
	//定数バッファの生成
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff)
	);

	//行列を初期化
	sprite.matRot = XMMatrixIdentity();
	sprite.matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	sprite.matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);

	//マップ処理
	Vertex* vertMap = nullptr;
	sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	//全頂点に対して
	for (int i = 0; i < sprite.vertices.size(); ++i)
	{
		vertMap[i] = sprite.vertices.at(i);
	}
	//マップを解除
	sprite.vertBuff->Unmap(0, nullptr);

	unsigned short* indexMap = nullptr;
	result = sprite.indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全頂点に対して
	for (int i = 0; i < sprite.indices.size(); ++i)
	{
		indexMap[i] = sprite.indices.at(i);
	}
	//マップを解除
	sprite.indexBuff->Unmap(0, nullptr);

	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		sprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = sprite.constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)sprite.constBuff->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;;
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

void SpriteManager::CommonInit(int blendID,D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType)
{
	//渡されたデータをどう使うかの解釈方法をGPUに伝えるためのもの
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{	//頂点座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};

	//シェーダー読み込み
	ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	//頂点シェーダーの読み込みコンパイル
	HRESULT result = D3DCompileFromFile(
		L"BasicVS.hlsl",									//シェーダーファイル
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		"main", "vs_5_0",									//エントリーポイント名、シェーダーモデル限定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用固定
		0,
		&vsBlob, &errorBlob
	);

	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",									//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		"main", "ps_5_0",									//エントリーポイント名、シェーダーモデル固定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用固定
		0,
		&psBlob, &errorBlob
	);

	//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeCBV{}, descRangeSRV{};
	descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);					//b0レジスタ
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);					//t0レジスタ

	//ルートパラメーターの設定
	CD3DX12_ROOT_PARAMETER rootparam[2]{};
	//定数用
	rootparam[0].InitAsDescriptorTable(1, &descRangeCBV, D3D12_SHADER_VISIBILITY_ALL);
	//テクスチャ用
	rootparam[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//グラフィックスパイプライン生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;						//標準設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//背面カリング,ポリゴン内塗りつぶし,深度クリッピングを有効に
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = gpipeline.BlendState.RenderTarget[0];
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.BlendEnable = true;											//ブレンドを有効にする
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;							//加算
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;								//ソースの値を100%使う
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;							//デストの値を0%使う

	switch (blendID) {
	case addblend:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;									//加算
		blendDesc.SrcBlend = D3D12_BLEND_ONE;									//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_ONE;									//ソースの値を100%使う
		break;
	case alphablend:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;									//加算
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;								//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;						//ソースの値を100%使う
		break;
	default:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;									//加算
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;								//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;						//ソースの値を100%使う
	}

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = primitiveType;

	//その他の設定
	gpipeline.NumRenderTargets = 1;							//描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1;											//1ピクセルにつき1回サンプリング

	//デプスステンシルステートの設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//深度テストを行う,書き込み許可,深度が小さければ合格
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//テクスチャサンプラーのセット
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparam), rootparam, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob = {};
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = DirectXBase::dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));

	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature.Get();

	result = DirectXBase::dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));

}

void SpriteManager::CommonDraw(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	//パイプラインステート設定コマンド
	DirectXBase::Instance()->cmdList->SetPipelineState(pipelinestate.Get());

	//ルートシグネチャ設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootSignature(rootsignature.Get());

	//プリミティブ形状設定コマンド
	DirectXBase::Instance()->cmdList->IASetPrimitiveTopology(topology);
}

void SpriteManager::Draw(Sprite& sprite)
{
	//非表示状態だったら描画処理を行わない
	if (sprite.isDisplay == false) return;

	//定数バッファへのデータ転送
	ConstBufferData* constMap = nullptr;
 	HRESULT result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite.constBufferData.color;
	
	//ワールド行列の更新
	if (sprite.projectionID == backGround)
	{
		constMap->mat.world = Camera::Instance()->matProjection;
	}
	else if (sprite.projectionID == object)
	{
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= sprite.matScale;
		matWorld *= sprite.matRot;
		matWorld *= sprite.matTrans;
		constMap->mat.world = matWorld * Camera::matView * Camera::matPerspective;				//ワールド変換 * ビュー変換 * 透視投影変換
		//constMap->mat.viewproj = matView * matProjection;
	}
	//ビルボードの場合
	else if (sprite.projectionID == billBoard) {
		//視点座標
		XMVECTOR eyePosition = XMLoadFloat3(&Camera::Instance()->eye);
		//注視点座標
		XMVECTOR targetPosition = XMLoadFloat3(&Camera::Instance()->target);
		//(仮の)上方向
		XMVECTOR upVector = XMLoadFloat3(&Camera::Instance()->up);
		//カメラZ軸
		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
		//0ベクトルだと向きが定まらないので除外
		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
		assert(!XMVector3IsInfinite(cameraAxisZ));
		assert(!XMVector3Equal(upVector, XMVectorZero()));
		assert(!XMVector3IsInfinite(upVector));
		//ベクトルを正規化
		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
		//カメラのX軸方向(右方向)
		XMVECTOR cameraAxisX{};
		//X軸は上方向->Z軸の外積で求まる
		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
		//ベクトルを正規化
		cameraAxisX = XMVector3Normalize(cameraAxisX);
		//カメラのY軸(上方向)
		XMVECTOR cameraAxisY{};
		//Y軸はZ軸->X軸の外積で求める
		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
		//ベクトルを正規化
		cameraAxisY = XMVector3Normalize(cameraAxisY);
		//カメラ回転行列
		XMMATRIX matCameraRot;
		//カメラ座標系->ワールド座標系の返還行列
		matCameraRot.r[0] = cameraAxisX;
		matCameraRot.r[1] = cameraAxisY;
		matCameraRot.r[2] = cameraAxisZ;
		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
		//転地により逆行列(逆回転)を計算
		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
		//視点座標に-1をかけた座標
		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
		//一つのベクトルにまとめる
		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
		//ビュー行列に平行移動分を設定
		matView.r[3] = translation;

		//ビルボード行列
		XMMATRIX billboardMat = XMMatrixIdentity();
		billboardMat.r[0] = cameraAxisX;
		billboardMat.r[1] = cameraAxisY;
		billboardMat.r[2] = cameraAxisZ;
		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
		//ワールド行列の更新
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= billboardMat;
		matWorld *= sprite.matScale;
		matWorld *= sprite.matRot;
		matWorld *= sprite.matTrans;
		constMap->mat.world = matWorld * matView * Camera::Instance()->matPerspective;												//ワールド変換 * ビュー変換 * 透視投影変換
		//constMap->mat.viewproj = 
	}
	sprite.constBuff->Unmap(0, nullptr);

	//ディスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps[] = { sprite.constDescHeap.Get() };
	DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//定数バッファビュー設定コマンド
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle;
	gpuDescHandle = sprite.constDescHeap->GetGPUDescriptorHandleForHeapStart();
	//gpuDescHandle.ptr += DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXBase::Instance()->cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	//ディスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Instance()->GetDescHeap().Get() };
	DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
	
	//シェーダーリソースビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Instance()->GetSRV(sprite.textureIDnum));
	

	//頂点バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);

	//インデックスバッファビュー設定コマンドを実行する
	if (sprite.indices.size() > 0) {
		DirectXBase::Instance()->cmdList->IASetIndexBuffer(&sprite.ibView);
	}

	//描画コマンド
	if (sprite.indices.size() > 0) {
		DirectXBase::Instance()->cmdList->DrawIndexedInstanced(sprite.indices.size(), 1, 0, 0, 0);
	}
	else {
		DirectXBase::Instance()->cmdList->DrawInstanced(sprite.vertices.size(), 1, 0, 0);
	}
}

void SpriteManager::ChangeScale(Sprite& sprite, XMFLOAT3 scale)
{
	sprite.matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
}

void SpriteManager::ChangeRotation(Sprite& sprite, XMFLOAT3 rotation)
{
	XMMATRIX mat = XMMatrixIdentity();
	mat *= XMMatrixRotationZ(rotation.z);
	mat *= XMMatrixRotationX(rotation.x);
	mat *= XMMatrixRotationY(rotation.y);
	sprite.matRot = mat * sprite.matRot;
}

void SpriteManager::ChangePosition(Sprite& sprite, XMFLOAT3 position)
{
	sprite.matTrans = XMMatrixTranslation(position.x, position.y, position.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);
}

void SpriteManager::ChangePositionAdd(Sprite& sprite, XMFLOAT3 position)
{
	sprite.matTrans *= XMMatrixTranslation(position.x, position.y, position.z);
	sprite.pos = XMFLOAT3(sprite.matTrans.r[3].m128_f32[0], sprite.matTrans.r[3].m128_f32[1], sprite.matTrans.r[3].m128_f32[2]);
}

void SpriteManager::Invertion(Sprite& sprite, bool isFlipX, bool isFlipY)
{
	sprite.isFlipX = isFlipX;
	sprite.isFlipY = isFlipY;

	if (sprite.isFlipX)
	{
		sprite.SavePos.x = -sprite.SavePos.x;
		sprite.SavePos.y = -sprite.SavePos.y;
	}

	if (sprite.isFlipY)
	{
		sprite.SavePos.z = -sprite.SavePos.z;
		sprite.SavePos.w = -sprite.SavePos.w;
	}
}
