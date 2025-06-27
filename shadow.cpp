//================================================
//
// 影 [shadow.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "shadow.h"
#include "manager.h"
#include"renderer.h"
#include"math.h"

using namespace math;

//================================================
// コンストラクタ
//================================================
CShadow::CShadow()
{
}

//================================================
// デストラクタ
//================================================
CShadow::~CShadow()
{
}

//================================================
// 生成処理
//================================================
CShadow* CShadow::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const D3DXCOLOR col)
{
	CShadow* pShadow = nullptr;

	// 影の生成
	pShadow = new CShadow;

	// 優先順位の取得
	int nPriority = pShadow->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pShadow != nullptr)
	{
		// 自分自身の破棄
		pShadow->Release();

		// nullにしておく
		pShadow = nullptr;

		return nullptr;
	}

	if (pShadow == nullptr) return nullptr;

	pShadow->SetPosition(pos);
	pShadow->SetSize(D3DXVECTOR3(fWidth,0.0f,fHeight));
	pShadow->Init();
	pShadow->SetOffsetVtx(col);

	return pShadow;
}

//================================================
// 初期化処理
//================================================
HRESULT CShadow::Init(void)
{
	if (FAILED(CObject3D::Init()))
	{
		return E_FAIL;
	}
	
	// テクスチャのIDの登録
	SetTextureID("data/TEXTURE/effect000.jpg");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CShadow::Uninit(void)
{
	CObject3D::Uninit();
}

//================================================
// 更新処理
//================================================
void CShadow::Update(void)
{

}

//================================================
// 描画処理
//================================================
void CShadow::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 透明度オプション追加
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CObject3D::Draw();

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//================================================
// 影の位置の設定処理
//================================================
void CShadow::Setting(const D3DXVECTOR3 player, const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fMaxHeight, const float fAlv)
{
	// 高さと最大の高さの割合を計算
	float fWidthWk = player.y / fMaxHeight;
	float fHeightWk = player.y / fMaxHeight;

	// 割合をもとに大きさを決める
	float SetWidth = fWidth + (fWidthWk * fWidth);
	float SetHeight = fHeight + (fHeightWk * fHeight);

	UpdatePosition(pos, D3DXVECTOR3(SetWidth, 0.0f, SetHeight));

	float fAvalue = player.y / fMaxHeight;

	UpdateCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlv - fAvalue));
}

//===================================================
// 地面に合わせた回転の取得処理
//===================================================
D3DXVECTOR3 CShadow::GetFieldAngle(D3DXVECTOR3 Nor, D3DXVECTOR3 up)
{
	D3DXVECTOR3 axis; // 回転軸

	// 軸を求めるて正規化する
	D3DXVec3Cross(&axis, &up, &Nor);
	D3DXVec3Normalize(&axis, &axis);

	// フィールドの法線と上方向ベクトルの内積して角度を求める
	float dot = D3DXVec3Dot(&up, &Nor);
	float fAngle = acosf(dot);

	D3DXMATRIX mtxRot; // 計算用回転行列

	// 任意の軸から回転行列を求める
	D3DXMatrixRotationAxis(&mtxRot, &axis, fAngle);

	// 回転行列をオイラー角に変換
	D3DXVECTOR3 rot = MatrixToEulerXYZ(mtxRot);

	return rot;
}
