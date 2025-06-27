//===================================================
//
// ビルボードの描画 [billboard.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "billboard.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CObjectBillboard::CObjectBillboard(int nPriority) : CObject(nPriority)
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));
	m_pos = VEC3_NULL;
	m_Size = VEC3_NULL;
	m_nTextureIdx = NULL;
	m_pVtxBuffer = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CObjectBillboard::~CObjectBillboard()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObjectBillboard::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;

}

//===================================================
// 終了処理
//===================================================
void CObjectBillboard::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CObjectBillboard::Update(void)
{
}

//===================================================
// 描画処理
//===================================================
void CObjectBillboard::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用のマトリックスを宣言
	D3DXMATRIX mtxRot, mtxTrans;

	// ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスを宣言
	D3DXMATRIX mtxView;

	// ビューマトリックスの取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// カメラの逆行列を設定
	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ライトを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//===================================================
// 頂点のオフセットの設定処理
//===================================================
void CObjectBillboard::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 大きさの取得
	D3DXVECTOR3 Size = m_Size;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / nPosX, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / nPosY);
	pVtx[3].tex = D3DXVECTOR2(1.0f / nPosX, 1.0f / nPosY);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 頂点の色の設定
//===================================================
void CObjectBillboard::SetColor(const D3DXCOLOR col)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// テクスチャのIDの設定
//===================================================
void CObjectBillboard::SetTextureID(const char* pTextureName)
{
	if (pTextureName == NULL)
	{
		m_nTextureIdx = -1;
		return;
	}
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// 頂点座標の更新
//===================================================
void CObjectBillboard::UpdateVertexPos(const D3DXVECTOR3 pos)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 位置の設定
	m_pos = pos;

	// 大きさの取得
	D3DXVECTOR3 Size = m_Size;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// テクスチャ座標の設定
//===================================================
void CObjectBillboard::SetTextureVtx(const D3DXVECTOR2 tex,const D3DXVECTOR2 offtex)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(offtex.x, offtex.y);
	pVtx[1].tex = D3DXVECTOR2(offtex.x + tex.x, offtex.y);
	pVtx[2].tex = D3DXVECTOR2(offtex.x, offtex.y + tex.y);
	pVtx[3].tex = D3DXVECTOR2(offtex.x + tex.x, offtex.y + tex.y);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 生成処理
//===================================================
CObjectBillboard* CObjectBillboard::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const char* pTextureName)
{
	CObjectBillboard* pBillboard = nullptr;
	
	// 3Dオブジェクトの生成
	pBillboard = new CObjectBillboard;

	// 優先順位の取得
	int nPriority = pBillboard->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pBillboard != nullptr)
	{
		// 自分自身の破棄
		pBillboard->Release();

		// nullにしておく
		pBillboard = nullptr;

		return nullptr;
	}

	if (pBillboard == nullptr) return nullptr;

	pBillboard->Init();
	pBillboard->m_pos = pos;
	pBillboard->m_Size = size;
	pBillboard->SetOffsetVtx();
	pBillboard->SetTextureID(pTextureName);
	return pBillboard;
}

//===================================================
// コンストラクタ
//===================================================
CBillboardAnimation::CBillboardAnimation(int nPriority) : CObjectBillboard(nPriority)
{
	m_fPosTexH = NULL;
	m_fPosTexU = NULL;
	m_nAnimSpeed = NULL;
	m_nCounterAnim = NULL;
	m_nPatternAnim = NULL;
	m_nU = NULL;
	m_nV = NULL;
}

//===================================================
// デストラクタ
//===================================================
CBillboardAnimation::~CBillboardAnimation()
{
}

//===================================================
// アニメーションの設定処理
//===================================================
void CBillboardAnimation::SetAnim(const int nAnimSpeed, const int U, const int V)
{
	m_nAnimSpeed = nAnimSpeed;
	m_nU = U;
	m_nV = V;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBillboardAnimation::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBillboardAnimation::Uninit(void)
{
	// ビルボードの破棄
	CObjectBillboard::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CBillboardAnimation::Update(void)
{
	// ビルボードの更新
	CObjectBillboard::Update();

	// アニメーションのカウンターを進める
	m_nCounterAnim++;

	m_fPosTexU = 1.0f / m_nU;
	m_fPosTexH = 1.0f / m_nV;

	if (m_nCounterAnim >= m_nAnimSpeed)
	{
		m_nCounterAnim = 0;

		m_nPatternAnim++;

		// Y座標の割合
		int nRatePosY = m_nPatternAnim / m_nU;

		float UV = m_fPosTexU * m_nPatternAnim;
		float HV = nRatePosY * m_fPosTexH;

		// テクスチャ座標の更新
		CObjectBillboard::SetTextureVtx(D3DXVECTOR2(m_fPosTexU, m_fPosTexH), D3DXVECTOR2(UV, HV));

		// 最大まで行ったら
		if (m_nPatternAnim >= (m_nU * m_nV))
		{
			Uninit();
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CBillboardAnimation::Draw(void)
{
	// ビルボードの描画
	CObjectBillboard::Draw();
}
