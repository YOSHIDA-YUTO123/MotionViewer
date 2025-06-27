//================================================
//
// メッシュドーム [dome.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "dome.h"
#include"manager.h"
#include"renderer.h"

//================================================
// コンストラクタ
//================================================
CMeshDome::CMeshDome()
{

}

//================================================
// デストラクタ
//================================================
CMeshDome::~CMeshDome()
{
}

//================================================
// 生成処理
//================================================
CMeshDome* CMeshDome::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// メッシュドームを生成
	CMeshDome* pMesh = new CMeshDome;

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
	int nNumFanVtx = (nSegX) + 1;

	// ポリゴン数の設定
	int nNumFanPolygon = (nNumFanVtx) * 3;

	int nNumIdxFan = nNumFanPolygon + 2;
	// 頂点数の設定
	int nNumDomeVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumDomePolygon = (((nSegX * nSegZ) * 2)) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumDomeIndex = nNumDomePolygon + 2;

	// 頂点情報の設定
	int nNumVtx = nNumFanVtx + nNumDomeVtx;

	int nNumIdx = nNumDomeIndex + nNumIdxFan;

	// 頂点の設定
	pMesh->SetVtxElement(nNumVtx, nNumDomePolygon, nNumIdx);

	pMesh->SetSegment(nSegX, nSegZ);
	
	// 初期化処理
	pMesh->Init();

	// 設定処理
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);

	pMesh->SetDome(nSegX, nSegZ, fRadius, fHeight);
	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshDome::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/sky.jpg");

	return E_FAIL;
}

//================================================
// 終了処理
//================================================
void CMeshDome::Uninit(void)
{
	//if (m_pIdxBuffMeshDome != nullptr)
	//{
	//	m_pIdxBuffMeshDome->Release();
	//	m_pIdxBuffMeshDome = nullptr;
	//}

	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshDome::Update(void)
{
	UpdateRotation(D3DXVECTOR3(0.0f,0.001f,0.0f));
}

//================================================
// 描画処理
//================================================
void CMeshDome::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 頂点数の設定
	int nNumFanVtx = nSegX + 1;

	// ポリゴン数の設定
	int nNumFanPolygon = nNumFanVtx * 3;

	// 頂点数の設定
	int nNumDomeVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumDomePolygon = (((nSegX * nSegZ) * 2)) + (4 * (nSegZ - 1));

	// 描画処理
	CMesh::SetUpDraw();

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, nNumFanVtx, 0, nNumFanPolygon);

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, nNumDomeVtx, m_nOffsetIdx, nNumDomePolygon);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// ドームの設定処理
//================================================
void CMeshDome::SetDome(const int nSegX, const int nSegZ, const float fRadius, const float fHeight)
{
	int nCntVtx = 0;
	float fTexX = 0.5f / nSegX;
	float fTexY = 0.5f / nSegZ;

	float fNowRadius = fRadius / (nSegZ + 1);

	// 蓋の部分の作成
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		D3DXVECTOR3 posWk = VEC3_NULL,posOld = VEC3_NULL;

		posWk.x = sinf(fAngle) * fNowRadius;
		posWk.y = fHeight;
		posWk.z = cosf(fAngle) * fNowRadius;

		float fPosTexX = fTexX + cosf(fAngle) * fTexX;
		float fPosTexY = fTexY - sinf(fAngle) * fTexY;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fPosTexX, fPosTexY));

		// 最後まで行ったら
		if (nCntX == nSegX)
		{
			//// 最後をつなげる
			//SetVtxBuffer(posOld, nCntVtx + 1, D3DXVECTOR2(fPosTexX, fPosTexY));
		}

		// 前回の計算を保存
		posOld = posWk;

		nCntVtx++;
	}
	
	int OffsetIdx = nCntVtx;

	// ドームのてっぺんの位置の設定
	SetVtxBuffer(D3DXVECTOR3(0.0f, fHeight + (fHeight / nSegZ + 1),0.0f), 0, D3DXVECTOR2(fTexX, fTexY));

	int nCntIdx = 0;

	for (int IndxCount2 = 0; IndxCount2 < nSegX; IndxCount2++)
	{
		// インデックスバッファの設定
		SetIndexBuffer((WORD)0, nCntIdx);
		SetIndexBuffer((WORD)IndxCount2 + 1, nCntIdx + 1);
		SetIndexBuffer((WORD)IndxCount2 + 2, nCntIdx + 2);

		nCntIdx += 3;
	}

	m_nOffsetIdx = nCntIdx;

	// テクスチャのオフセット
	fTexX = 1.0f / nSegX;
	fTexY = 1.0f / nSegZ;

	// 半径
	fNowRadius = fRadius / (nSegZ + 1);
	float fRateRadius = fNowRadius;

	//縦
	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		//横
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			float fAngel = (D3DX_PI * 2.0f) / nSegX * nCntX;

			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngel) * fNowRadius;
			posWk.y = fHeight - (fHeight / nSegZ) * nCntZ;
			posWk.z = cosf(fAngel) * fNowRadius;
			
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexX * nCntX, fTexY * nCntZ));

			nCntVtx++;//加算
		}
		fNowRadius += fRateRadius / (nCntZ + 1);
	}

	int IndxCount3 = nSegX + 1;//X

	int IdxCnt = m_nOffsetIdx;//配列

	int Num = 0;//

	int Index0 = 0;
	int Index1 = 0;

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegX; IndxCount2++, IndxCount3++, Num++)
		{
			Index0 = IndxCount3 + OffsetIdx;
			Index1 = Num + OffsetIdx;

			// インデックスバッファの設定
			SetIndexBuffer((WORD)Index0, IdxCnt);
			SetIndexBuffer((WORD)Index1, IdxCnt + 1);
			IdxCnt += 2;
		}

		if (IndxCount1 < nSegZ - 1)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)(Num - 1 + OffsetIdx), IdxCnt);
			SetIndexBuffer((WORD)(IndxCount3 + OffsetIdx), IdxCnt + 1);
			IdxCnt += 2;
		}
	}

}
