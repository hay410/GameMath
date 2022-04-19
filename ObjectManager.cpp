#include"ObjectManager.h"

vector<ObjectData> ObjectManager::objectData = {};

void ObjectManager::LoadObject(int dataID, Sprite& sprite, const char* fileName)
{
	//図形が生成済みかどうかを確認する
	bool isLoad = false;
	int dataNum = 0;
	for (int i = 0; i < objectData.size(); ++i)
	{
		if (objectData.at(i).dataID == dataID)
		{
			isLoad = true;
			dataNum = i;
		}
	}

	if (isLoad == false)
	{
		switch (dataID)
		{
		case Object:
			CreateObject(fileName, sprite);
			break;
		case Cone:
			CreateCone(sprite);
			break;
		case Cylinder:
			CreateCylinder(sprite);
			break;
		case Sphere:
			CreateSphere(sprite);
			break;
		default:
			break;
		}
	}
	else
	{
		for (int i = 0; i < objectData.at(dataNum).indices.size(); ++i)
		{
			sprite.indices.push_back(objectData.at(dataNum).indices.at(i));
		}
		for (int i = 0; i < objectData.at(dataNum).vertices.size(); ++i)
		{
			sprite.vertices.push_back(objectData.at(dataNum).vertices.at(i));
		}
	}
}

void ObjectManager::CreateObject(const char* fileName,Sprite& sprite)
{
	objectData.push_back({});
	objectData.at(objectData.size() - 1).dataID = Cone;
	//ファイルストリーム
	ifstream file;
	//objファイルを開く
	file.open(fileName);
	//ファイルオープン失敗をチェック
	if (file.fail())assert(0);

	vector<XMFLOAT3> positions;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> uv;

	//1行ずつ読み込む
	string line;
	while (getline(file,line))
	{
		//一行分の文字列をストリームに変換して解析しやすくする
		istringstream line_stream(line);
		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' '); 

		/*--------頂点情報の読み込み--------*/

		//先頭文字列がvなら頂点
		if (key == "v")
		{
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.push_back(position);
			//座標データに追加
			//vertices vertices;
			//vertices.pos = position;
			//sprite.vertices.push_back(vertices);
		}

		//先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			//U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			uv.push_back(texcoord);
		}

		//先頭文字列がvnなら法線ベクトル
		if (key == "vn")
		{
			//xyz成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルにデータ追加
			normals.push_back(normal);
		}

		//先頭文字がfならインデックス
		if (key == "f")
		{
			//半角スペース区切りで行の続きを読み取る
			string indices_string;

			while (getline(line_stream, indices_string, ' '))
			{
				//頂点インデックス一個分の文字列をストリームに変換して解析しやすくする
				istringstream indices_stream(indices_string);
				unsigned short indicesPos, indicesUv, IndicesNormal;
				indices_stream >> indicesPos;
				indices_stream.seekg(1, ios_base::cur);
				indices_stream >> indicesUv;
				indices_stream.seekg(1, ios_base::cur);
				indices_stream >> IndicesNormal;
				Vertex vertices{};
				vertices.pos = positions[indicesPos - 1];
				vertices.normal = normals[IndicesNormal - 1];
				vertices.uv = uv[indicesUv - 1];
				sprite.vertices.push_back(vertices);
				objectData.at(objectData.size() - 1).vertices.push_back(vertices);
				//インデックスデータの追加
				sprite.indices.push_back((unsigned short)sprite.indices.size());
				objectData.at(objectData.size() - 1).indices.push_back((unsigned short)sprite.indices.size());
			}
		}
	}
	//ファイルを閉じる
	file.close();
}

void ObjectManager::CreateCone(Sprite& sprite)
{
	//objectDataを生成
	objectData.push_back({});
	objectData.at(objectData.size() - 1).dataID = Cone;

	int verticesCount = 30;			//頂点数
	//底面の中心以外の座標を計算
	for (int i = 0; i < verticesCount - 2; ++i)
	{
		Vertex pos;
		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (verticesCount - 2) * i),
			1 * (cosf(XM_2PI / (verticesCount - 2) * i)),
			0);
		pos.uv = XMFLOAT2(0, 0);
		objectData.at(objectData.size() - 1).vertices.push_back(pos);
		sprite.vertices.push_back(pos);
	}

	//底面の中心
	Vertex circleCenterPos{};
	circleCenterPos.pos = XMFLOAT3(0, 0, 0);
	circleCenterPos.uv = XMFLOAT2(0, 0);

	objectData.at(objectData.size() - 1).vertices.push_back(circleCenterPos);
	sprite.vertices.push_back(circleCenterPos);

	//頂点
	Vertex topHeightPos{};
	topHeightPos.pos = XMFLOAT3(0, 0, -5);
	topHeightPos.uv = XMFLOAT2(0, 0);

	objectData.at(objectData.size() - 1).vertices.push_back(topHeightPos);

	sprite.vertices.push_back(topHeightPos);

	//底面のインデックスの設定
	int i = 0;
	for (; i < verticesCount - 3; ++i) {
		objectData.at(objectData.size() - 1).indices.push_back(i);
		objectData.at(objectData.size() - 1).indices.push_back(verticesCount - 2);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1);

		sprite.indices.push_back(i);
		sprite.indices.push_back(verticesCount - 2);
		sprite.indices.push_back(i + 1);
	}

	objectData.at(objectData.size() - 1).indices.push_back(i);
	objectData.at(objectData.size() - 1).indices.push_back(verticesCount - 2);
	objectData.at(objectData.size() - 1).indices.push_back(0);

	sprite.indices.push_back(i);
	sprite.indices.push_back(verticesCount - 2);
	sprite.indices.push_back(0);

	//側面のインデックスの設定
	i = 0;
	for (; i < verticesCount - 3; ++i) {
		objectData.at(objectData.size() - 1).indices.push_back(i);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1);
		objectData.at(objectData.size() - 1).indices.push_back(verticesCount - 1);

		sprite.indices.push_back(i);
		sprite.indices.push_back(i + 1);
		sprite.indices.push_back(verticesCount - 1);
	}

	objectData.at(objectData.size() - 1).indices.push_back(i);
	objectData.at(objectData.size() - 1).indices.push_back(0);
	objectData.at(objectData.size() - 1).indices.push_back(verticesCount - 1);

	sprite.indices.push_back(i);
	sprite.indices.push_back(0);
	sprite.indices.push_back(verticesCount - 1);

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
		//objectDataへ保存
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(index0).normal, normal);
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(index1).normal, normal);
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(index2).normal, normal);
		//spriteへのコピーも行う
		XMStoreFloat3(&sprite.vertices.at(index0).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index1).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index2).normal, normal);
	}
}

void ObjectManager::CreateCylinder(Sprite& sprite)
{
	//objectDataを生成
	objectData.push_back({});
	objectData.at(objectData.size() - 1).dataID = Cylinder;
	//頂点バッファの生成
	int verticesCount = 30;			//頂点数
	//底面の座標を計算
	for (int i = 0; i < verticesCount / 2 - 1; ++i) {
		Vertex pos;
		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (verticesCount / 2 - 1) * i),
			1 * (cosf(XM_2PI / (verticesCount / 2 - 1) * i)),
			0);
		pos.uv = XMFLOAT2(0, 0);
		//objectDataへ保存
		objectData.at(objectData.size() - 1).vertices.push_back(pos);
		//spriteへのデータコピーも行う
		sprite.vertices.push_back(pos);
	}
	Vertex circleCenterPos{};
	circleCenterPos.pos = XMFLOAT3(0, 0, 0);
	circleCenterPos.uv = XMFLOAT2(0, 0);
	//objectDataへ保存
	objectData.at(objectData.size() - 1).vertices.push_back(circleCenterPos);
	//spriteへのデータコピーも行う
	sprite.vertices.push_back(circleCenterPos);
	//天面の座標を計算
	for (int i = 0; i < verticesCount / 2 - 1; ++i) {
		Vertex pos;
		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (verticesCount / 2 - 1) * i),
			1 * (cosf(XM_2PI / (verticesCount / 2 - 1) * i)),
			-1);
		pos.uv = XMFLOAT2(0, 0);
		//objectDataへ保存
		objectData.at(objectData.size() - 1).vertices.push_back(pos);
		//spriteへのデータコピーも行う
		sprite.vertices.push_back(pos);
	}
	Vertex topHeightPos{};
	topHeightPos.pos = XMFLOAT3(0, 0, -1);
	topHeightPos.uv = XMFLOAT2(0, 0);
	//objectDataへ保存
	objectData.at(objectData.size() - 1).vertices.push_back(topHeightPos);
	//spriteへのデータコピーも行う
	sprite.vertices.push_back(topHeightPos);

	//底面のインデックスの設定
	int i = 0;
	for (; i < verticesCount / 2 - 2; ++i) {
		//objectDataへ保存
		objectData.at(objectData.size() - 1).indices.push_back(i);
		objectData.at(objectData.size() - 1).indices.push_back(verticesCount / 2 - 1);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1);
		//spriteへのデータコピーも行う
		sprite.indices.push_back(i);
		sprite.indices.push_back(verticesCount / 2 - 1);
		sprite.indices.push_back(i + 1);
	}
	//objectDataへ保存
	objectData.at(objectData.size() - 1).indices.push_back(i);
	objectData.at(objectData.size() - 1).indices.push_back(verticesCount / 2 - 1);
	objectData.at(objectData.size() - 1).indices.push_back(0);
	//spriteへのデータコピーも行う
	sprite.indices.push_back(i);
	sprite.indices.push_back(verticesCount / 2 - 1);
	sprite.indices.push_back(0);
	//天面のインデックスの設定
	i = 0;
	for (; i < verticesCount / 2 - 2; ++i) {
		//objectDataへ保存
		objectData.at(objectData.size() - 1).indices.push_back(i + verticesCount / 2);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1 + verticesCount / 2);
		objectData.at(objectData.size() - 1).indices.push_back(verticesCount - 1);
		//spriteへのデータコピーも行う
		sprite.indices.push_back(i + verticesCount / 2);
		sprite.indices.push_back(i + 1 + verticesCount / 2);
		sprite.indices.push_back(verticesCount - 1);
	}
	//objectDataへ保存
	objectData.at(objectData.size() - 1).indices.push_back(i + verticesCount / 2);
	objectData.at(objectData.size() - 1).indices.push_back(verticesCount / 2);
	objectData.at(objectData.size() - 1).indices.push_back(verticesCount - 1);
	//spriteへのデータコピーも行う
	sprite.indices.push_back(i + verticesCount / 2);
	sprite.indices.push_back(verticesCount / 2);
	sprite.indices.push_back(verticesCount - 1);
	//側面のインデックスの設定
	i = 0;
	for (; i < verticesCount / 2 - 2; ++i) {
		//objectDataへ保存
		objectData.at(objectData.size() - 1).indices.push_back(i);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1);
		objectData.at(objectData.size() - 1).indices.push_back(i + verticesCount / 2);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1);
		objectData.at(objectData.size() - 1).indices.push_back(i + 1 + verticesCount / 2);
		objectData.at(objectData.size() - 1).indices.push_back(i + verticesCount / 2);
		//spriteへのデータコピーも行う
		sprite.indices.push_back(i);
		sprite.indices.push_back(i + 1);
		sprite.indices.push_back(i + verticesCount / 2);
		sprite.indices.push_back(i + 1);
		sprite.indices.push_back(i + 1 + verticesCount / 2);
		sprite.indices.push_back(i + verticesCount / 2);
	}
	//objectDataへ保存
	objectData.at(objectData.size() - 1).indices.push_back(i);
	objectData.at(objectData.size() - 1).indices.push_back(0);
	objectData.at(objectData.size() - 1).indices.push_back(i + verticesCount / 2);
	objectData.at(objectData.size() - 1).indices.push_back(0);
	objectData.at(objectData.size() - 1).indices.push_back(verticesCount / 2);
	objectData.at(objectData.size() - 1).indices.push_back(i + verticesCount / 2);
	//spriteへのデータコピーも行う
	sprite.indices.push_back(i);
	sprite.indices.push_back(0);
	sprite.indices.push_back(i + verticesCount / 2);
	sprite.indices.push_back(0);
	sprite.indices.push_back(verticesCount / 2);
	sprite.indices.push_back(i + verticesCount / 2);

	//法線ベクトルの計算
	for (int i = 0; i < sprite.indices.size() / 3; ++i) {
		//三角形ひとつひとつ計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indices0 = sprite.indices.at(i * 3 + 0);
		unsigned short indices1 = sprite.indices.at(i * 3 + 1);
		unsigned short indices2 = sprite.indices.at(i * 3 + 2);
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&sprite.vertices.at(indices0).pos);
		XMVECTOR p1 = XMLoadFloat3(&sprite.vertices.at(indices1).pos);
		XMVECTOR p2 = XMLoadFloat3(&sprite.vertices.at(indices2).pos);
		//p0->p1ベクトル,p0->p2ベクトルを計算
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外戚は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		//objectDataへ保存
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(indices0).normal, normal);
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(indices1).normal, normal);
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(indices2).normal, normal);
		//spriteへのデータコピーも行う
		XMStoreFloat3(&sprite.vertices.at(indices0).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(indices1).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(indices2).normal, normal);
	}
}

void ObjectManager::CreateSphere(Sprite& sprite)
{
	//objectDataを生成
	objectData.push_back({});
	objectData.at(objectData.size() - 1).dataID = Sphere;
	//頂点バッファの生成
	int vertexCount = 30;			//頂点数
	Vertex pos;
	//底面の座標
	pos.pos = XMFLOAT3(0, 0, -1);
	pos.uv = XMFLOAT2(0, 0);
	//objectDataに保存
	objectData.at(objectData.size() - 1).vertices.push_back(pos);
	//spriteにもデータをコピー
	sprite.vertices.push_back(pos);
	//側面の座標の計算
	for (int i = 1; i < vertexCount; ++i) {
		for (int j = 0; j < vertexCount; ++j) {
			float circleRadius = (1 * sinf(XM_PI / vertexCount * i));
			pos.pos = XMFLOAT3(circleRadius * sinf(XM_2PI / vertexCount * j),
				circleRadius * (cosf(XM_2PI / vertexCount * j)),
				-1 * (cosf((XM_PI / vertexCount * i)))
			);
			pos.uv = XMFLOAT2(0, 0);
			//objectDataに保存
			objectData.at(objectData.size() - 1).vertices.push_back(pos);
			//spriteにもデータをコピー
			sprite.vertices.push_back(pos);
		}
	}
	//天面の座標
	pos.pos = XMFLOAT3(0, 0, 1);
	pos.uv = XMFLOAT2(0, 0);
	//objectDataに保存
	objectData.at(objectData.size() - 1).vertices.push_back(pos);
	//spriteにもデータをコピー
	sprite.vertices.push_back(pos);

	//底面のインデックスの設定
	int buff = 1;
	for (int i = 0; i < vertexCount - 1; ++i) {
		//objectDataに保存
		objectData.at(objectData.size() - 1).indices.push_back(buff);
		objectData.at(objectData.size() - 1).indices.push_back(buff + 1);
		objectData.at(objectData.size() - 1).indices.push_back(0);
		//spriteにもデータをコピー
		sprite.indices.push_back(buff);
		sprite.indices.push_back(buff + 1);
		sprite.indices.push_back(0);
		buff++;
	}
	//objectDataに保存
	objectData.at(objectData.size() - 1).indices.push_back(buff);
	objectData.at(objectData.size() - 1).indices.push_back(1);
	objectData.at(objectData.size() - 1).indices.push_back(0);
	//spriteにもデータをコピー
	sprite.indices.push_back(buff);
	sprite.indices.push_back(1);
	sprite.indices.push_back(0);
	buff++;
	//側面のインデックスの設定
	for (int i = 0; i < vertexCount - 2; ++i) {
		for (int j = 0; j < vertexCount - 1; ++j) {
			//objectDataに保存
			objectData.at(objectData.size() - 1).indices.push_back(buff);
			objectData.at(objectData.size() - 1).indices.push_back(buff + 1);
			objectData.at(objectData.size() - 1).indices.push_back(buff - (vertexCount - 1));
			objectData.at(objectData.size() - 1).indices.push_back(buff);
			objectData.at(objectData.size() - 1).indices.push_back(buff - (vertexCount - 1));
			objectData.at(objectData.size() - 1).indices.push_back(buff - vertexCount);
			//spriteにもデータをコピー
			sprite.indices.push_back(buff);
			sprite.indices.push_back(buff + 1);
			sprite.indices.push_back(buff - (vertexCount - 1));
			sprite.indices.push_back(buff);
			sprite.indices.push_back(buff - (vertexCount - 1));
			sprite.indices.push_back(buff - vertexCount);
			buff++;
		}
		//objectDataに保存
		objectData.at(objectData.size() - 1).indices.push_back(buff);
		objectData.at(objectData.size() - 1).indices.push_back(buff - (vertexCount - 1));
		objectData.at(objectData.size() - 1).indices.push_back(buff - (vertexCount - 1) - vertexCount);
		objectData.at(objectData.size() - 1).indices.push_back(buff);
		objectData.at(objectData.size() - 1).indices.push_back(buff - (vertexCount - 1) - vertexCount);
		objectData.at(objectData.size() - 1).indices.push_back(buff - vertexCount);
		//spriteにもデータをコピー
		sprite.indices.push_back(buff);
		sprite.indices.push_back(buff - (vertexCount - 1));
		sprite.indices.push_back(buff - (vertexCount - 1) - vertexCount);
		sprite.indices.push_back(buff);
		sprite.indices.push_back(buff - (vertexCount - 1) - vertexCount);
		sprite.indices.push_back(buff - vertexCount);
		buff++;
	}
	//天面のインデックスの設定
	for (int i = vertexCount - 1; i > 0; --i) {
		//objectDataに保存
		objectData.at(objectData.size() - 1).indices.push_back(sprite.vertices.size() - 1 - i);
		objectData.at(objectData.size() - 1).indices.push_back(sprite.vertices.size() - 1 - i - 1);
		objectData.at(objectData.size() - 1).indices.push_back(sprite.vertices.size() - 1);
		//spriteにもデータをコピー
		sprite.indices.push_back(sprite.vertices.size() - 1 - i);
		sprite.indices.push_back(sprite.vertices.size() - 1 - i - 1);
		sprite.indices.push_back(sprite.vertices.size() - 1);
	}
	//objectDataに保存
	objectData.at(objectData.size() - 1).indices.push_back(sprite.vertices.size() - 1 - 1);
	objectData.at(objectData.size() - 1).indices.push_back(sprite.vertices.size() - 1);
	objectData.at(objectData.size() - 1).indices.push_back(sprite.vertices.size() - 1 - vertexCount);
	//spriteにもデータをコピー
	sprite.indices.push_back(sprite.vertices.size() - 1 - 1);
	sprite.indices.push_back(sprite.vertices.size() - 1);
	sprite.indices.push_back(sprite.vertices.size() - 1 - vertexCount);

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
		//objectDataに保存
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(index0).normal, normal);
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(index1).normal, normal);
		XMStoreFloat3(&objectData.at(objectData.size() - 1).vertices.at(index2).normal, normal);
		//spriteにもデータをコピー
		XMStoreFloat3(&sprite.vertices.at(index0).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index1).normal, normal);
		XMStoreFloat3(&sprite.vertices.at(index2).normal, normal);
	}
}
