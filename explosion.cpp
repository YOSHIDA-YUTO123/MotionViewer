//===================================================
//
// 爆発,煙 [explosion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "explosion.h"
#include "manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CExplosion::CExplosion(int nPriority) : CBillboardAnimation(nPriority)
{
	m_col = WHITE;
}

//===================================================
// デストラクタ
//===================================================
CExplosion::~CExplosion()
{
}

//===================================================
// 生成処理
//===================================================
CExplosion* CExplosion::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed)
{
	// 爆発の生成
	CExplosion* pExplotion = new CExplosion;

	// 優先順位の取得
	int nPriority = pExplotion->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pExplotion != nullptr)
	{
		// 自分のポインタの解放
		pExplotion->Release();

		// nullにする
		pExplotion = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pExplotion == nullptr) return nullptr;

	pExplotion->m_col = col;
	pExplotion->SetAnim(nAnimSpeed, U, V);
	pExplotion->Init();
	pExplotion->GetPosition()->Set(pos);
	pExplotion->GetSize()->Set(size);
	pExplotion->SetOffsetVtx(col, U, V);

	return pExplotion;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CExplosion::Init(void)
{
	if (FAILED(CBillboardAnimation::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("data/TEXTURE/explosion001.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CExplosion::Uninit(void)
{
	// 終了処理
	CBillboardAnimation::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CExplosion::Update(void)
{
	// 更新処理
	CBillboardAnimation::Update();
}

//===================================================
// 描画処理
//===================================================
void CExplosion::Draw(void)
{
	// 更新処理
	CBillboardAnimation::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CDustSmoke::CDustSmoke(int nPriority) : CObjectBillboard(nPriority)
{
	m_col = WHITE;
	m_move = VEC3_NULL;
}

//===================================================
// デストラクタ
//===================================================
CDustSmoke::~CDustSmoke()
{
}

//===================================================
// 生成処理
//===================================================
CDustSmoke* CDustSmoke::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col)
{
	// 煙の生成
	CDustSmoke* pExplotion = new CDustSmoke;

	// 優先順位の取得
	int nPriority = pExplotion->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pExplotion != nullptr)
	{
		// 自分のポインタの解放
		pExplotion->Release();

		// nullにする
		pExplotion = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pExplotion == nullptr) return nullptr;

	pExplotion->m_col = col;
	pExplotion->Init();
	pExplotion->GetPosition()->Set(pos);
	pExplotion->GetSize()->Set(size);
	pExplotion->SetOffsetVtx(col, 1, 1);

	return pExplotion;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CDustSmoke::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	SetTextureID("data/TEXTURE/smoke.jpg");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CDustSmoke::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CDustSmoke::Update(void)
{
	// 位置の更新処理
	GetPosition()->UpdatePosition(m_move);
}

//===================================================
// 描画処理
//===================================================
void CDustSmoke::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//// アルファテストを有効
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CObjectBillboard::Draw();

	//ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//// アルファテストを無効
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// aブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
