//================================================
//
// メッシュ [mesh.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "mesh.h"
#include "manager.h"
#include "renderer.h"

//================================================
// コンストラクタ
//================================================
CMesh::CMesh()
{
	m_pVtxBuffMesh = nullptr;
	m_pIdxBuffMesh = nullptr;
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nSegmentX = NULL;	
	m_nSegmentZ = NULL;	
	m_nIdexTexture = NULL;
	m_nNumVtx = NULL;	
	m_nNumPolygon = NULL;
	m_nNumIndex = NULL;	
}

//================================================
// デストラクタ
//================================================
CMesh::~CMesh()
{
}

//================================================
// 初期化処理
//================================================
HRESULT CMesh::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffMesh,
		NULL)))
	{
		return E_FAIL;
	}

	//インデックスバッファの生成
	if (FAILED(pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuffMesh,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMesh::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffMesh != nullptr)
	{
		m_pVtxBuffMesh->Release();
		m_pVtxBuffMesh = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuffMesh != nullptr)
	{
		m_pIdxBuffMesh->Release();
		m_pIdxBuffMesh = nullptr;
	}

	// 自分自身の破棄
	Release();
}

//================================================
// 描画処理
//================================================
void CMesh::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデバイスのデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffMesh, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuffMesh);

	//テクスチャフォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャが無かったら
	if (m_nIdexTexture == -1)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, NULL);
	}
	else
	{
		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAdress(m_nIdexTexture));

	}

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0, m_nNumPolygon);
}

//================================================
// 描画処理
//================================================
void CMesh::SetUpDraw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデバイスのデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffMesh, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuffMesh);

	//テクスチャフォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャが無かったら
	if (m_nIdexTexture == -1)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, NULL);
	}
	else
	{
		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAdress(m_nIdexTexture));

	}
}

//================================================
// 頂点座標の設定
//================================================
void CMesh::SetVtxBuffer(const D3DXVECTOR3 pos, const int nIdx, const D3DXVECTOR2 tex, const D3DXVECTOR3 nor, const D3DXCOLOR col)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].pos = pos;

	//法線ベクトルの設定
	pVtx[nIdx].nor = nor;

	//頂点カラーの設定
	pVtx[nIdx].col = col;

	//テクスチャ座標の設定
	pVtx[nIdx].tex = tex;

	// 頂点バッファをアンロック
	m_pVtxBuffMesh->Unlock();
}

//================================================
// インデックスバッファの設定
//================================================
void CMesh::SetIndexBuffer(const WORD Idx,const int nCnt)
{
	WORD* pIdx;

	//インデックスバッファのロック
	m_pIdxBuffMesh->Lock(0, 0, (void**)&pIdx, 0);

	pIdx[nCnt] = Idx;

	//インデックスバッファのアンロック
	m_pIdxBuffMesh->Unlock();
}

//================================================
// テクスチャの設定処理
//================================================
void CMesh::SetTextureID(const char* pTextureName)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	if (pTextureName == NULL)
	{
		m_nIdexTexture = -1;
		return;
	}

	// テクスチャのIDの設定
	m_nIdexTexture = pTexture->Register(pTextureName);
}

//================================================
// 頂点の要素の設定処理
//================================================
void CMesh::SetVtxElement(const int vertex, const int polygon, const int index)
{
	// 頂点数の設定
	m_nNumVtx = vertex;

	// ポリゴン数の設定
	m_nNumPolygon = polygon;

	// インデックス数の設定
	m_nNumIndex = index;
}

//================================================
// 分割数の設定処理
//================================================
void CMesh::SetSegment(const int nSegX, const int nSegZ)
{
	m_nSegmentX = nSegX;
	m_nSegmentZ = nSegZ;
}

//================================================
// 頂点座標の取得
//================================================
D3DXVECTOR3 CMesh::GetVtxPos(const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	D3DXVECTOR3 out;

	// 頂点バッファをロック
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	out = pVtx[nIdx].pos;

	// 頂点バッファをアンロック
	m_pVtxBuffMesh->Unlock();

	return out;
}

//================================================
// インデックス番号の取得
//================================================
int CMesh::GetIndex(const int nIdx)
{
	WORD* pIdx;

	int out = 0;
	//インデックスバッファのロック
	m_pIdxBuffMesh->Lock(0, 0, (void**)&pIdx, 0);

	out = pIdx[nIdx];

	//インデックスバッファのアンロック
	m_pIdxBuffMesh->Unlock();

	return out;
}

//================================================
// 頂点の位置の設定
//================================================
void CMesh::SetVtxPos(const D3DXVECTOR3 pos,const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].pos = pos;

	// 頂点バッファをアンロック
	m_pVtxBuffMesh->Unlock();
}

//================================================
// 法線の設定
//================================================
void CMesh::SetNormal(const D3DXVECTOR3 nor, const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].nor = nor;

	// 頂点バッファをアンロック
	m_pVtxBuffMesh->Unlock();
}

//================================================
// 色の設定
//================================================
void CMesh::SetColor(const D3DXCOLOR col,const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].col = col;

	// 頂点バッファをアンロック
	m_pVtxBuffMesh->Unlock();
}

//================================================
// 法線の正規化処理
//================================================
D3DXVECTOR3 CMesh::NormalizeNormal(const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	// 法線
	D3DXVECTOR3 nor = pVtx[nIdx].pos - m_pos;

	D3DXVec3Normalize(&nor, &nor);

	// 頂点バッファをアンロック
	m_pVtxBuffMesh->Unlock();

	return nor;
}
