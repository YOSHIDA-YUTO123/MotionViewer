//================================================
//
// メッシュインパクト [impact.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "impact.h"
#include "manager.h"
#include "renderer.h"

//================================================
// コンストラクタ
//================================================
CMeshCircle::CMeshCircle()
{
	m_Incol = WHITE;
	m_Outcol = WHITE;
	m_fDecAlv = NULL;
	m_fOutRadius = NULL;
	m_fSpeed = NULL;
	m_nLife = NULL;
}

//================================================
// デストラクタ
//================================================
CMeshCircle::~CMeshCircle()
{
}

//================================================
// 生成処理
//================================================
CMeshCircle* CMeshCircle::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const float InRadius, const float OutRadius, const float speed, const int nLife, const D3DXCOLOR col, const D3DXVECTOR3 rot)
{
	// メッシュインパクトを生成
	CMeshCircle* pMesh = new CMeshCircle;

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
	pMesh->m_fInRadius = InRadius;
	pMesh->m_fOutRadius = OutRadius;
	pMesh->m_nLife = nLife;
	pMesh->m_fSpeed = speed;
	pMesh->m_Outcol = col;
	pMesh->m_Incol = D3DXCOLOR(col.r, col.g, col.b, col.a * 0.5f);
	pMesh->m_fDecAlv = col.a / nLife;

	pMesh->SetImpact(nSegX, InRadius, OutRadius);

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshCircle::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID();

	return E_FAIL;
}

//================================================
// 終了処理
//================================================
void CMeshCircle::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshCircle::Update(void)
{
	int nCntVtx = 0;

	int nSegX = GetSegX();

	float fTexPosX = 1.0f / nSegX;

	m_fInRadius += m_fSpeed;
	m_fOutRadius += m_fSpeed;

	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		posWk.x = sinf(fAngle) * m_fInRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * m_fInRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f),D3DXVECTOR3(0.0f,1.0f,0.0f),m_Incol);

		nCntVtx++;
	}

	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		posWk.x = sinf(fAngle) * m_fOutRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * m_fOutRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), m_Outcol);

		nCntVtx++;
	}

	m_Incol.a -= m_fDecAlv;
	m_Outcol.a -= m_fDecAlv;

	m_nLife--;

	if (m_nLife <= 0)
	{
		// 終了処理
		Uninit();

		return;
	}
}

//================================================
// 描画処理
//================================================
void CMeshCircle::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CMesh::Draw();

	// aブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// インパクトの設定処理
//================================================
void CMeshCircle::SetImpact(const int nSegX,const float InRadius, const float OutRadius)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f / nSegX;
	D3DXVECTOR3 posWk = VEC3_NULL;

	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		posWk.x = sinf(fAngle) * InRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * InRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		nCntVtx++;
	}

	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		posWk.x = sinf(fAngle) * OutRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * OutRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		nCntVtx++;
	}

	int IndxNum = nSegX + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

	// Zの分割数
	int nSegZ = GetSegZ();

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
