#pragma once
#include"struct.h"

namespace HHelper
{
	/*---- 便利変数 ----*/

	static const int COLOR_MAX = 255;														//色の最大値
	static const XMFLOAT3 COLOR_RED_VIVID = XMFLOAT3(COLOR_MAX, 0, 0);						//RGB振り切りの赤
	static const XMFLOAT3 COLOR_GREEN_VIVID = XMFLOAT3(0, COLOR_MAX, 0);					//RGB振り切りの緑
	static const XMFLOAT3 COLOR_BLUE_VIVID = XMFLOAT3(0, 0, COLOR_MAX);						//RGB振り切りの青
	static const XMFLOAT3 COLOR_CYAN_VIVID = XMFLOAT3(0, COLOR_MAX, COLOR_MAX);				//RGB振り切りのシアン
	static const XMFLOAT3 COLOR_MAGENTA_VIVID = XMFLOAT3(COLOR_MAX, 0, COLOR_MAX);			//RGB振り切りのマゼンタ
	static const XMFLOAT3 COLOR_YELLOW_VIVID = XMFLOAT3(COLOR_MAX, COLOR_MAX, 0);				//RGB振り切りのイエロー
	static const XMFLOAT3 COLOR_BLACK_VIVID = XMFLOAT3(0, 0, 0);							//RGB振り切りの黒
	static const XMFLOAT3 COLOR_WHITE_VIVID = XMFLOAT3(COLOR_MAX, COLOR_MAX, COLOR_MAX);	//RGB振り切りの白

	static const float F_PI_F = 3.14159265359;												//円周率 180度
	static const float F_PI_DIV_F = F_PI_F / 2.0f;											//円周率 90度
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;									//円周率 270度
	static const float F_2PI_F = F_PI_F * 2.0f;												//円周率 360度

	/*---- 便利関数 ----*/

	//2D座標軸での二点間の距離を求める
	inline float Distance2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f));
	}
	//3D座標軸での二点間の距離を求める
	inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
	}

	//内積を求める
	inline float Dot2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
	inline float Dot3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	//外積を求める
	inline float Cross2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}
	inline XMFLOAT3 Cross3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		//タイプミスしてる可能性あり
		return XMFLOAT3(lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x);
	}

	//ベクトルの長さを求める
	inline float Length2D(const XMFLOAT2& rhs) {
		return sqrtf(Dot2D(rhs, rhs));
	}
	inline float Length3D(const XMFLOAT3& rhs) {
		return sqrtf(Dot3D(rhs, rhs));
	}

	//ベクトルを正規化する
	inline XMFLOAT2 Normalize2D(const XMFLOAT2& rhs) {
		float len = Length2D(rhs);
		XMFLOAT2 buff = rhs;
		buff.x /= len;
		buff.y /= len;
		return buff;
	}
	inline XMFLOAT3 Normalize3D(const XMFLOAT3& rhs) {
		float len = Length3D(rhs);
		XMFLOAT3 buff = rhs;
		buff.x /= len;
		buff.y /= len;
		buff.z /= len;
		return buff;
	}

	//引数を足す
	inline XMFLOAT2 AddXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	inline XMFLOAT3 AddXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	//引数を引く
	inline XMFLOAT2 SubXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x - rhs.x, lhs.y - rhs.y);
	}
	inline XMFLOAT3 SubXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		XMFLOAT3 buff = XMFLOAT3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);

		//限りなく0に近かったら0にする
		if (fabs(buff.x) < 0.00001f) {
			buff.x = 0;
		}
		if (fabs(buff.y) < 0.00001f) {
			buff.y = 0;
		}
		if (fabs(buff.z) < 0.00001f) {
			buff.z = 0;
		}

		return buff;
	}

	//引数をかける
	inline XMFLOAT2 MulXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x * rhs.x, lhs.y * rhs.y);
	}
	inline XMFLOAT3 MulXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	//引数を割る
	inline XMFLOAT2 DivXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x / rhs.x, lhs.y / rhs.y);
	}
	inline XMFLOAT3 DivXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

	//乱数取得
	inline int GetRand(const int& min, const int& max) {
		return (rand() % (max - min + 1)) + min;
	}
	inline XMFLOAT3 GetRandXMFLOAT3(const int& min, const int& max) {
		return XMFLOAT3(GetRand(min, max), GetRand(min, max), GetRand(min, max));
	}

	//画面中央
	inline XMFLOAT3 WindowCenterPos() {
		return XMFLOAT3(WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f, 0);
	}

	//画面サイズの半分
	inline XMFLOAT2 WindowHalfSize() {
		return XMFLOAT2(WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f);
	}

	//レイとObject3Dの当たり判定
	inline bool RayToModelCollision(const Sprite& target, const Ray& ray, XMFLOAT3& impactPos, float& distance, XMFLOAT3& hitNormal) {
		//あたっているポリゴンのデータを保存するよう変数	衝突地点、距離、衝突面の法線
		vector<tuple<XMFLOAT3, float, XMFLOAT3>> buff{};

		float epsilon = 1.0e-5f;
		/*-----targetからポリゴンを抜き取る-----*/
		vector<Porygon> targetPorygon;		//ポリゴン保存用コンテナ

		//targetのポリゴン数に合わせてリサイズ
		targetPorygon.resize(target.indices.size() / 3.0f);

		//ポリゴンの中身を代入
		for (int i = 0; i < targetPorygon.size(); ++i) {
			targetPorygon.at(i).p1 = target.vertices.at(target.indices.at(i * 3));
			targetPorygon.at(i).p2 = target.vertices.at(target.indices.at((i * 3) + 1));
			targetPorygon.at(i).p3 = target.vertices.at(target.indices.at((i * 3) + 2));
			targetPorygon.at(i).isActive = true;
		}

		/*-----保存したポリゴンの頂点座標にワールド変換行列をかける-----*/
		//ワールド行列
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= target.matScale;
		matWorld *= target.matRot;
		matWorld *= target.matTrans;
		for (int i = 0; i < targetPorygon.size(); ++i) {
			//頂点を変換
			XMStoreFloat3(&targetPorygon.at(i).p1.pos, XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p1.pos), matWorld));
			XMStoreFloat3(&targetPorygon.at(i).p2.pos, XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p2.pos), matWorld));
			XMStoreFloat3(&targetPorygon.at(i).p3.pos, XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p3.pos), matWorld));
			//法線を回転行列分だけ変換
			XMStoreFloat3(&targetPorygon.at(i).p1.normal, XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p1.normal), target.matRot)));
			XMStoreFloat3(&targetPorygon.at(i).p2.normal, XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p2.normal), target.matRot)));
			XMStoreFloat3(&targetPorygon.at(i).p3.normal, XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p3.normal), target.matRot)));
		}

		/*-----レイの方向と法線が同じ方向なら除外-----*/
		for (int i = 0; i < targetPorygon.size(); ++i) {
			//まずは1つ目の頂点をチェック
			float kari = XMVector3Dot(XMLoadFloat3(&targetPorygon.at(i).p1.normal), ray.dir).m128_f32[0];
			if (XMVector3Dot(XMLoadFloat3(&targetPorygon.at(i).p1.normal), ray.dir).m128_f32[0] > -0.0001f) {
				//ここまで来たら完全に反対側を向いているので、削除フラグを建てる
				targetPorygon.at(i).isActive = false;
				continue;
			}
		}

		/*-----ポリゴンの3頂点と境界球の当たり判定を行い、一定以上距離があった場合は処理を飛ばす-----*/

		/*-----ポリゴンごとに当たり判定-----*/
		//此処から先はポリゴンごとに計算する
		for (int i = 0; i < targetPorygon.size(); ++i) {
			/*-----レイと平面の衝突点を計算する-----*/

			//ポリゴンが無効化されていたら次の処理へ
			if (targetPorygon.at(i).isActive == false) continue;

			//レイの開始地点から平面におろした垂線の長さを求める
			XMFLOAT3 planeNorm = { -targetPorygon.at(i).p1.normal.x,-targetPorygon.at(i).p1.normal.y,-targetPorygon.at(i).p1.normal.z };
			float rayToOriginLength = XMVector3Dot(ray.pos, XMLoadFloat3(&planeNorm)).m128_f32[0];
			float planeToOriginLength = XMVector3Dot(XMLoadFloat3(&targetPorygon.at(i).p1.pos), XMLoadFloat3(&planeNorm)).m128_f32[0];
			//視点から平面におろした垂線の長さ
			float perpendicularLine = rayToOriginLength - planeToOriginLength;

			//三角関数を利用して視点から衝突店までの距離を求める
			float dist = XMVector3Dot(XMLoadFloat3(&planeNorm), ray.dir).m128_f32[0];
			float impDistance = perpendicularLine / -dist;

			//distanceが0未満の場合交わっていない
			/*if (impDistance < 0) {
				targetPorygon.at(i).isActive = false;
				continue;
			}*/

			//衝突地点
			XMFLOAT3 impactPoint = { ray.pos.m128_f32[0] + ray.dir.m128_f32[0] * impDistance,
									ray.pos.m128_f32[1] + ray.dir.m128_f32[1] * impDistance,
									ray.pos.m128_f32[2] + ray.dir.m128_f32[2] * impDistance };

			/*-----衝突点がポリゴンの内側にあるかを調べる-----*/
			XMVECTOR m;

			/*辺1本目*/
			XMVECTOR impactToP1 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p1.pos.x - impactPoint.x, targetPorygon.at(i).p1.pos.y - impactPoint.y, targetPorygon.at(i).p1.pos.z - impactPoint.z));
			XMVECTOR P2ToP1 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p2.pos.x - targetPorygon.at(i).p1.pos.x, targetPorygon.at(i).p2.pos.y - targetPorygon.at(i).p1.pos.y, targetPorygon.at(i).p2.pos.z - targetPorygon.at(i).p1.pos.z));
			m = XMVector3Cross(impactToP1, P2ToP1);
			//正規化
			m = XMVector3Normalize(m);
			//法線と一致しているか調べる
			float result = XMVector3Dot(m, XMLoadFloat3(&targetPorygon.at(i).p1.normal)).m128_f32[0];
			if (result <= 0) {
				//結果が0以外だったら衝突地点がポリゴンの内側ではないので、衝突していない
				targetPorygon.at(i).isActive = false;
				continue;
			}

			/*辺2本目*/
			XMVECTOR impactToP2 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p2.pos.x - impactPoint.x, targetPorygon.at(i).p2.pos.y - impactPoint.y, targetPorygon.at(i).p2.pos.z - impactPoint.z));
			XMVECTOR P3ToP2 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p3.pos.x - targetPorygon.at(i).p2.pos.x, targetPorygon.at(i).p3.pos.y - targetPorygon.at(i).p2.pos.y, targetPorygon.at(i).p3.pos.z - targetPorygon.at(i).p2.pos.z));
			m = XMVector3Cross(impactToP2, P3ToP2);
			//正規化
			m = XMVector3Normalize(m);
			//法線と一致しているか調べる
			result = XMVector3Dot(m, XMLoadFloat3(&targetPorygon.at(i).p1.normal)).m128_f32[0];
			if (result <= 0) {
				//結果が0以外だったら衝突地点がポリゴンの内側ではないので、衝突していない
				targetPorygon.at(i).isActive = false;
				continue;
			}

			/*辺3本目*/
			XMVECTOR impactToP3 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p3.pos.x - impactPoint.x, targetPorygon.at(i).p3.pos.y - impactPoint.y, targetPorygon.at(i).p3.pos.z - impactPoint.z));
			XMVECTOR P1ToP3 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p1.pos.x - targetPorygon.at(i).p3.pos.x, targetPorygon.at(i).p1.pos.y - targetPorygon.at(i).p3.pos.y, targetPorygon.at(i).p1.pos.z - targetPorygon.at(i).p3.pos.z));
			m = XMVector3Cross(impactToP3, P1ToP3);
			//正規化
			m = XMVector3Normalize(m);
			//法線と一致しているか調べる
			result = XMVector3Dot(m, XMLoadFloat3(&targetPorygon.at(i).p1.normal)).m128_f32[0];
			if (result <= 0) {
				//結果が0以外だったら衝突地点がポリゴンの内側ではないので、衝突していない
				targetPorygon.at(i).isActive = false;
				continue;
			}

			/*ここまで来たらポリゴンに衝突してる！*/
			tuple<XMFLOAT3, float, XMFLOAT3> buffer{};
			get<0>(buffer) = impactPoint;
			get<1>(buffer) = impDistance;
			get<2>(buffer) = targetPorygon.at(i).p1.normal;
			buff.push_back(buffer);
		}

		//buffの値が1以上だったら距離が最小の要素を検索
		if (buff.size() >= 1) {
			//距離が最小の要素を検索
			int min = 0;
			float minDistance = 100000;
			for (int i = 0; i < buff.size(); ++i) {
				if (fabs(get<1>(buff.at(i))) < fabs(minDistance)) {
					minDistance = get<1>(buff.at(i));
					min = i;
				}
			}

			//検索した最小値を代入してreturn
			impactPos = get<0>(buff.at(min));
			distance = get<1>(buff.at(min));
			hitNormal = get<2>(buff.at(min));
			return true;
		}
		else {
			impactPos = { 0,0,0 };
			distance = 0;
			hitNormal = { 0,0,0 };
			return false;
		}
	}

	//画面内に収まっているかを判定
	inline bool CheckInScreen(const XMFLOAT3& checkPos, const float& virtualWidth, const float& virtualHeight, const XMMATRIX& matView, const XMMATRIX& matPerspective) {
		float w = WIN_WIDTH / 2.0f;
		float h = WIN_HEIGHT / 2.0f;
		XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// ビュー変換とプロジェクション変換
		XMVECTOR worldPos = XMLoadFloat3(&checkPos);
		worldPos = XMVector3Transform(worldPos, matView);
		worldPos = XMVector3Transform(worldPos, matPerspective);

		XMFLOAT3 temp;
		XMStoreFloat3(&temp, worldPos);
		// zで割って-1~1の範囲に収める
		// スクリーン変換
		XMVECTOR viewVec = XMVectorSet(temp.x / temp.z, temp.y / temp.z, 1.0f, 1.0f);
		viewVec = XMVector3Transform(viewVec, viewport);
		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, viewVec);


		if (pos.x > -virtualWidth && pos.x < WIN_WIDTH + virtualWidth && pos.y > -virtualHeight && pos.y < WIN_HEIGHT + virtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}

	inline bool SphereCollision(Sprite left, Sprite right)
	{
		//ワールド座標系を行列から読み取る
		XMFLOAT3 position1 = { left.matTrans.r[3].m128_f32[0],left.matTrans.r[3].m128_f32[1],left.matTrans.r[3].m128_f32[2] };
		XMFLOAT3 position2 = { right.matTrans.r[3].m128_f32[0],right.matTrans.r[3].m128_f32[1],right.matTrans.r[3].m128_f32[2] };
		//２つの座標の差
		XMVECTOR position_sub = XMVectorSet(position1.x - position2.x,
			position1.y - position2.y,
			position1.z - position2.z,
			0);
		//２つの座標の距離
		position_sub = XMVector3Length(position_sub);
		float distance = position_sub.m128_f32[0];

		//距離が半径の和以下ならあたっている
		if (distance <= left.radius + right.radius) {
			return true;
		}
		return false;
	}

}