//===================================================
//
// 3Dオブジェクトの描画 [object3D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "object3D.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CObject3D::CObject3D()
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));
	m_pos = VEC3_NULL;
	m_pVtxBuffer = NULL;
	m_rot = VEC3_NULL;
	m_Size = VEC3_NULL;
	m_nTextureIdx = NULL;
}

//===================================================
// デストラクタ
//===================================================
CObject3D::~CObject3D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObject3D::Init(void)
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
void CObject3D::Uninit(void)
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
void CObject3D::Update(void)
{
	//CPlayer* pPlayer = CManager::GetPlayer();

	//D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	//// 頂点情報のポインタ
	//VERTEX_3D* pVtx;

	//// 頂点バッファのロック
	//m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	//D3DXVECTOR3 nor, vec1, vec2;
	//float fHeight = 0.0f;

	//vec1 = pVtx[2].pos - pVtx[0].pos;
	//vec2 = pVtx[1].pos - pVtx[0].pos;
	//D3DXVec3Cross(&nor, &vec2, &vec1);
	//D3DXVec3Normalize(&nor, &nor);

	//D3DXVECTOR3 vec3, vec4, vec5;

	//vec3 = pVtx[0].pos - pVtx[2].pos;
	//vec4 = pVtx[1].pos - pVtx[0].pos;
	//vec5 = pVtx[2].pos - pVtx[1].pos;

	//D3DXVec3Normalize(&vec3, &vec3);
	//D3DXVec3Normalize(&vec4, &vec4);
	//D3DXVec3Normalize(&vec5, &vec5);

	//D3DXVECTOR3 vecP0, vecP1, vecP2;

	//vecP0 = PlayerPos - pVtx[2].pos;
	//vecP1 = PlayerPos - pVtx[0].pos;
	//vecP2 = PlayerPos - pVtx[1].pos;

	//D3DXVec3Normalize(&vecP0, &vecP0);
	//D3DXVec3Normalize(&vecP1, &vecP1);
	//D3DXVec3Normalize(&vecP2, &vecP2);

	//D3DXVECTOR3 cross0, cross1, cross2;

	//D3DXVec3Cross(&cross0, &vec3, &vecP0);
	//D3DXVec3Cross(&cross1, &vec4, &vecP1);
	//D3DXVec3Cross(&cross2, &vec5, &vecP2);

	//if (cross0.y >= 0.0f && cross1.y >= 0.0f && cross2.y >= 0.0f)
	//{
	//	if (nor.y != 0.0f)
	//	{
	//		float xz = ((PlayerPos.x - pVtx[0].pos.x) * nor.x + (PlayerPos.z - pVtx[0].pos.z) * nor.z);

	//		fHeight = pVtx[0].pos.y - (xz / nor.y);

	//		pPlayer->SetPosition(D3DXVECTOR3(PlayerPos.x,m_pos.y + fHeight, PlayerPos.z));
	//	}
	//}

	//// 頂点バッファのアンロック
	//m_pVtxBuffer->Unlock();
}

//===================================================
// 描画処理
//===================================================
void CObject3D::Draw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//	ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===================================================
// 描画に必要な情報だけの描画処理
//===================================================
void CObject3D::SetDraw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

}

//===================================================
// 頂点のオフセットの設定処理
//===================================================
void CObject3D::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y + m_Size.y, m_pos.z + m_Size.z);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y + m_Size.y, m_pos.z + m_Size.z);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y - m_Size.y, m_pos.z - m_Size.z);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y - m_Size.y, m_pos.z - m_Size.z);

	D3DXVECTOR3 vtx0, vtx1;

	D3DXVECTOR3 nor, vec1, vec2;

	vec1 = pVtx[0].pos - pVtx[2].pos;
	vec2 = pVtx[3].pos - pVtx[2].pos;

	D3DXVec3Cross(&nor, &vec1, &vec2);
	D3DXVec3Normalize(&nor, &nor);

	// 法線ベクトルの設定
	pVtx[0].nor = nor;
	pVtx[1].nor = nor;
	pVtx[2].nor = nor;
	pVtx[3].nor = nor;

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
// テクスチャのIDの設定
//===================================================
void CObject3D::SetTextureID(const char* pTextureName)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// テクスチャの名前の設定
	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// 位置の更新処理
//===================================================
void CObject3D::UpdatePosition(const D3DXVECTOR3 pos,const D3DXVECTOR3 Size)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	m_pos = pos;
	m_Size = Size;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_Size.x, m_Size.y, m_Size.z);
	pVtx[1].pos = D3DXVECTOR3(m_Size.x, m_Size.y, m_Size.z);
	pVtx[2].pos = D3DXVECTOR3(-m_Size.x, -m_Size.y, -m_Size.z);
	pVtx[3].pos = D3DXVECTOR3(m_Size.x, -m_Size.y, -m_Size.z);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 色の更新処理
//===================================================
void CObject3D::UpdateCol(const D3DXCOLOR col)
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
// 生成処理
//===================================================
CObject3D* CObject3D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 size, const char* pTextureName)
{
	CObject3D* pObject3D = nullptr;

	// 3Dオブジェクトの生成
	pObject3D = new CObject3D;

	// 優先順位の取得
	int nPriority = pObject3D->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pObject3D != nullptr)
	{
		// 自分自身の破棄
		pObject3D->Release();

		// nullにしておく
		pObject3D = nullptr;

		return nullptr;
	}

	if (pObject3D == nullptr) return nullptr;

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	pObject3D->SetPosition(pos);
	pObject3D->SetRotaition(rot);
	pObject3D->SetSize(size);
	pObject3D->Init();
	pObject3D->SetOffsetVtx();
	pObject3D->m_nTextureIdx = pTexture->Register(pTextureName);

	return pObject3D;
}
