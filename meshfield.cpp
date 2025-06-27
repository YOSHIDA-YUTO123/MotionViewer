//================================================
//
// メッシュフィールド [meshfield.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "meshfield.h"
#include"manager.h"
#include "math.h"
#include"json_v3_10_5.hpp"
#include<fstream>

using json = nlohmann::json;

//************************************************
// マクロ定義
//************************************************
#define NUM_POLYGON (2)		 // 判定するポリゴンの数
#define NULL_VECTOR (999.0f) // vectorの空配列

//================================================
// コンストラクタ
//================================================
CMeshField::CMeshField()
{
	m_fWidth = NULL;
	m_fHeight = NULL;
	m_Nor = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CMeshField::~CMeshField()
{
}

//================================================
// 生成処理
//================================================
CMeshField* CMeshField::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const D3DXVECTOR2 Size, const D3DXVECTOR3 rot)
{
	// メッシュフィールドを生成
	CMeshField* pMeshField = new CMeshField;

	// 優先順位の取得
	int nPriority = pMeshField->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pMeshField != nullptr)
	{
		// 自分のポインタの解放
		pMeshField->Release();

		// nullにする
		pMeshField = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pMeshField == nullptr) return nullptr;
	
	// 頂点数の設定
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMeshField->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMeshField->SetSegment(nSegX, nSegZ);

	// 初期化処理
	pMeshField->Init();

	// 設定処理
	pMeshField->SetPosition(pos);
	pMeshField->SetRotation(rot);
	pMeshField->SetMeshField(nSegX, nSegZ, pos, Size);
	pMeshField->m_fWidth = Size.x;
	pMeshField->m_fHeight = Size.y;

	return pMeshField;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshField::Init(void)
{
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/field.png");

	//json config;

	//config["Loop"] = false;
	//config["NumKey"] = 5;

	//std::ofstream file("data/config.json");

	//file << config.dump(4); // インデント付きで保存
	//
	//std::fstream File("data/config.json");

	//bool bLoop = config["Loop"];
	//int nNumKey = config["NumKey"];

	//File.close();
	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshField::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshField::Update(void)
{
	// 法線の再設定
	UpdateNor();
}

//================================================
// 描画処理
//================================================
void CMeshField::Draw(void)
{
	// 描画処理
	CMesh::Draw();
}

//================================================
// メッシュフィールドの設定処理
//================================================
void CMeshField::SetMeshField(const int nSegX, const int nSegZ, const D3DXVECTOR3 pos,const D3DXVECTOR2 Size)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f / nSegX;
	float fTexPosY = 1.0f / nSegZ;

	D3DXVECTOR3 posWk;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// 位置の設定
			posWk.x = ((Size.x / nSegX) * nCntX) - (Size.x * 0.5f);
			posWk.y = pos.y;
			posWk.z = Size.y - ((Size.y / nSegZ) * nCntZ) - (Size.y * 0.5f);

			// 頂点バッファの設定
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)),D3DXVECTOR3(0.0f,0.0f,0.0f));

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1;//X

	int IdxCnt = 0;//配列

	int Num = 0;//

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegX; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)IndxNum, IdxCnt);
			SetIndexBuffer((WORD)Num, IdxCnt + 1);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < nSegZ - 1)
		{
			SetIndexBuffer((WORD)Num - 1, IdxCnt);
			SetIndexBuffer((WORD)IndxNum, IdxCnt + 1);
			IdxCnt += 2;
		}
	}
}

//================================================
// メッシュフィールドの当たり判定
//================================================
bool CMeshField::Collision(const D3DXVECTOR3 pos,float *pOutHeight)
{
	// 着地判定
	bool bLanding = false;

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 1マスのサイズ
	float GridSizeX = m_fWidth / (float)nSegX;
	float GridSizeZ = m_fHeight / (float)nSegZ;

	float X = pos.x + (m_fWidth * 0.5f);
	float Z = (m_fHeight * 0.5f) - pos.z;

	// 何番目のポリゴンか
	int polyX = (int)(X / GridSizeX);
	int polyZ = (int)(Z / GridSizeZ);

	// 現在のポリゴンのインデックス番号
	int polyIndex = ((polyZ * (nSegX - 1) + polyX) * 2) + (polyZ * 6);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	for (int nCnt = 0; nCnt < NUM_POLYGON; nCnt++)
	{
		// 頂点のインデックス
		int nCntVertex = (polyIndex + nCnt);

		// マイナスだったら
		if (nCntVertex < 0 || nCntVertex > nNumIndex) break;

		// インデックスを取得
		int nIdx0 = GetIndex(nCntVertex);
		int nIdx1 = GetIndex(nCntVertex + 1);
		int nIdx2 = GetIndex(nCntVertex + 2);

		// 頂点を取得
		D3DXVECTOR3 vtx0 = GetVtxPos(nIdx0);
		D3DXVECTOR3 vtx1 = GetVtxPos(nIdx1);
		D3DXVECTOR3 vtx2 = GetVtxPos(nIdx2);

		D3DXVECTOR3 edge0 = vtx1 - vtx0; // 辺ベクトル0
		D3DXVECTOR3 edge1 = vtx2 - vtx1; // 辺ベクトル1
		D3DXVECTOR3 edge2 = vtx0 - vtx2; // 辺ベクトル2

		D3DXVECTOR3 Normal = {};

		if (nCnt % 2 == 0)
		{
			// 偶数番目の三角形
			D3DXVec3Cross(&Normal, &edge0, &edge1);
		}
		else
		{
			// 奇数番目の三角形（順序が逆になっている）
			D3DXVec3Cross(&Normal, &edge1, &edge0);
		}

		D3DXVec3Normalize(&Normal, &Normal);
		m_Nor = Normal;

		D3DXVECTOR3 PlayerVec0 = pos - vtx0;
		D3DXVECTOR3 PlayerVec1 = pos - vtx1;
		D3DXVECTOR3 PlayerVec2 = pos - vtx2;

		D3DXVECTOR3 Cross0 = {};
		D3DXVECTOR3 Cross1 = {};
		D3DXVECTOR3 Cross2 = {};

		if (nCnt % 2 == 0)
		{
			// 三角形の頂点から外積
			D3DXVec3Cross(&Cross0, &edge0, &PlayerVec0);
			D3DXVec3Normalize(&Cross0, &Cross0);

			D3DXVec3Cross(&Cross1, &edge1, &PlayerVec1);
			D3DXVec3Normalize(&Cross1, &Cross1);

			D3DXVec3Cross(&Cross2, &edge2, &PlayerVec2);
			D3DXVec3Normalize(&Cross2, &Cross2);
		}
		else
		{
			// 三角形の頂点から外積
			D3DXVec3Cross(&Cross0, &PlayerVec0, &edge0);
			D3DXVec3Normalize(&Cross0, &Cross0);

			D3DXVec3Cross(&Cross1, &PlayerVec1, &edge1);
			D3DXVec3Normalize(&Cross1, &Cross1);

			D3DXVec3Cross(&Cross2, &PlayerVec2, &edge2);
			D3DXVec3Normalize(&Cross2, &Cross2);
		}

		if (Cross0.y >= 0.0f && Cross1.y >= 0.0f && Cross2.y >= 0.0f)
		{
			//float xz = ((pPos->x - vtx0.x) * Normal.x + (pPos->z - pPos->z) * Normal.z);

			//float fHeight = vtx0.y - (xz / Normal.y);

			// 平面の方程式のDを計算
			float D = -(Normal.x * vtx0.x + Normal.y * vtx0.y + Normal.z * vtx0.z);

			// プレイヤーの位置に基づいて、プレイヤーのY位置を計算
			float PosY = (Normal.x * pos.x + Normal.z * pos.z + D) / -Normal.y;

			D3DXVECTOR3 field = GetPosition();

			D3DXVECTOR3 vec = vtx0 - pos;
			D3DXVec3Normalize(&vec, &vec);

			// プレイヤーがポリゴンの裏側かどうかの判定
			float dot = D3DXVec3Dot(&Normal, &vec); // 法線とプレイヤー方向との内積

			*pOutHeight = field.y + PosY;

			if (dot > 0.0f)
			{
				bLanding = true;
				break;
			}
		}
	}

	return bLanding;//判定を返す
}

//================================================
// 法線の再設定処理
//================================================
void CMeshField::UpdateNor(void)
{
	int nCnt = 0;
	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 頂点数分調べる
	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// 計算用頂点0,1,2,3,4
			D3DXVECTOR3 vtx0, vtx1, vtx2, vtx3, vtx4;

			// 計算用頂点を初期化
			vtx0 = vtx1 = vtx2 = vtx3 = vtx4 = VEC3_NULL;

			// 計算用ベクトル0,1,2,3
			D3DXVECTOR3 vec0, vec1, vec2, vec3;

			// 計算用ベクトルの初期化
			vec0 = vec1 = vec2 = vec3 = VEC3_NULL;

			// 計算用インデックス0,1,2,3の宣言
			int nIdx0 = 0;
			int nIdx1 = 0;
			int nIdx2 = 0;
			int nIdx3 = 0;

			// 計算用法線0,1,2,3
			D3DXVECTOR3 Nor0, Nor1, Nor2, Nor3;

			// 計算用法線の初期化
			Nor0 = Nor1 = Nor2 = Nor3 = VEC3_NULL;

			// 計算後の法線格納用変数
			D3DXVECTOR3 Normal = VEC3_NULL;

			// 左の上だったら
			if (nCntX == 0)
			{
				// 左上だったら
				if (nCntZ == 0)
				{
					nIdx0 = 0;
					nIdx1 = 1;
					nIdx2 = nSegX + 1;

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// 左の辺(角以外)だったら
				else if (nCnt == (nSegX + 1) * nCntZ)
				{
					nIdx0 = nCnt - (nSegX + 1);
					nIdx1 = nCnt + 1;
					nIdx2 = nCnt + (nSegX + 1);

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);
					vtx3 = GetVtxPos(nCnt);

					vec0 = vtx0 - vtx3;
					vec1 = vtx1 - vtx3;
					vec2 = vtx2 - vtx3;

					D3DXVec3Cross(&Nor0, &vec0, &vec1);
					D3DXVec3Cross(&Nor1, &vec1, &vec2);

					Normal = (Nor0 + Nor1) * 0.5f;
				}
			}
			// 上の辺だったら
			else if (nCntZ == 0 && nCnt < nSegX)
			{
				nIdx0 = (nSegX + 1) + nCntX;
				nIdx1 = nCnt - 1;
				nIdx2 = nCnt + 1;

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);
				vtx3 = GetVtxPos(nIdx0);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;
				vec2 = vtx3 - vtx0;

				D3DXVec3Cross(&Nor0, &vec2, &vec0);
				D3DXVec3Cross(&Nor1, &vec1, &vec2);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// 左下だったら
			else if (nCntZ == nSegZ && nCntX == 0)
			{
				nIdx0 = (nSegX + 1) * nSegZ;
				nIdx1 = (nSegX + 1) * (nSegZ - 1);
				nIdx2 = ((nSegX + 1) * nSegZ) + 1;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec0, &vec1);
			}
			// 右上だったら
			else if (nCntX == nSegX && nCntZ == 0)
			{
				nIdx0 = nSegX;
				nIdx1 = nSegX - 1;
				nIdx2 = nSegX + 1 + nCntX;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// 下の辺だったら
			else if (nCntZ == nSegZ && nCnt < ((nSegX + 1) * (nSegZ + 1)) - 1)
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegX + 1);
				nIdx2 = nCnt + 1;

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx0);
				vtx2 = GetVtxPos(nIdx1);
				vtx3 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;
				vec2 = vtx3 - vtx0;

				D3DXVec3Cross(&Nor0, &vec0, &vec1);
				D3DXVec3Cross(&Nor1, &vec1, &vec2);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// 右の辺(角以外)だったら
			else if (nCntX == nSegX && nCnt == (nCntX * (nCntZ + 1)) + nCntZ)
			{
				nIdx0 = nCnt - (nSegX + 1);
				nIdx1 = nCnt - 1;
				nIdx2 = nCntX + (nSegX + 1);

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);
				vtx3 = GetVtxPos(nCnt);

				vec0 = vtx0 - vtx3;
				vec1 = vtx1 - vtx3;
				vec2 = vtx2 - vtx3;

				D3DXVec3Cross(&Nor0, &vec1, &vec0);
				D3DXVec3Cross(&Nor1, &vec2, &vec1);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// 右下だったら
			else if (nCnt == ((nSegX + 1) * (nSegZ + 1)) - 1)
			{
				nIdx0 = ((nSegX + 1) * (nSegZ + 1)) - 1;
				nIdx1 = nIdx0 - (nSegX + 1);
				nIdx2 = nIdx0 - 1;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// それ以外(端っこでも角でもない)
			else
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegX + 1);
				nIdx2 = nCnt + 1;
				nIdx3 = nCnt + (nSegX + 1);

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx0);
				vtx2 = GetVtxPos(nIdx1);
				vtx3 = GetVtxPos(nIdx2);
				vtx4 = GetVtxPos(nIdx3);

				vec0 = vtx1 - vtx0; // 左
				vec1 = vtx2 - vtx0; // 上

				vec2 = vtx3 - vtx0; // 右
				vec3 = vtx4 - vtx0; // 下

				D3DXVec3Cross(&Nor0, &vec0, &vec1);
				D3DXVec3Cross(&Nor1, &vec3, &vec0);
				D3DXVec3Cross(&Nor2, &vec1, &vec2);
				D3DXVec3Cross(&Nor3, &vec2, &vec3);

				Normal = (Nor0 + Nor1 + Nor2 + Nor3) * 0.25f;
			}
			D3DXVec3Normalize(&Normal, &Normal);

			SetNormal(Normal, nCnt);
			nCnt++;
		}
	}
}
