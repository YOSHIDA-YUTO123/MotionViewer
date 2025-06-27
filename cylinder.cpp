//================================================
//
// メッシュシリンダー [cylinder.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "cylinder.h"
#include "player.h"

//================================================
// コンストラクタ
//================================================
CMeshCylinder::CMeshCylinder()
{
}

//================================================
// デストラクタ
//================================================
CMeshCylinder::~CMeshCylinder()
{
}

//================================================
// 生成処理
//================================================
CMeshCylinder* CMeshCylinder::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ,const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// メッシュフィールドを生成
	CMeshCylinder* pMesh = new CMeshCylinder;

	// 優先順位の取得
	int nPriority = pMesh->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pMesh != nullptr)
	{
		// 自分のポインタの解放
		pMesh->Release();

		// nullにする
		pMesh = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pMesh == nullptr) return nullptr;

	// 頂点数の設定
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMesh->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMesh->SetSegment(nSegX, nSegZ);

	// 初期化処理
	pMesh->Init();

	// 設定処理
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);
	pMesh->SetCylinder(nSegX, nSegZ, fRadius, fHeight);

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshCylinder::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/field.jpg");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshCylinder::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshCylinder::Update(void)
{
}

//================================================
// 描画処理
//================================================
void CMeshCylinder::Draw(void)
{
	// 描画処理
	CMesh::Draw();
}

//================================================
// シリンダーの設定処理
//================================================
void CMeshCylinder::SetCylinder(const int nSegX, const int nSegZ, const float fRadius,const float fHeight)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f / nSegX;
	float fTexPosY = 1.0f / nSegZ;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			D3DXVECTOR3 posWk = VEC3_NULL;

			// 横の分割数
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			posWk.x = sinf(fAngle) * fRadius;
			posWk.y = fHeight - (fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * fRadius;

			// 頂点バッファの設定
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)));

			// 法線の正規化
			NormalizeNormal(nCntVtx);

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

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
