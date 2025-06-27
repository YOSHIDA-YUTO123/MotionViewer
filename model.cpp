//===================================================
//
// モデル [model.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "model.h"
#include "object.h"
#include "modelManager.h"
#include "manager.h"
#include"renderer.h"
#include<stdio.h>

//===================================================
// コンストラクタ
//===================================================
CModel::CModel()
{
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_offpos = VEC3_NULL;
	m_offrot = VEC3_NULL;
	m_pParent = nullptr;
	m_nModelIdx = NULL;
	m_pTextureIdx = nullptr;
	m_nParentIndex = NULL;
	memset(&m_Pose, NULL, sizeof(m_Pose));

}

//===================================================
// デストラクタ
//===================================================
CModel::~CModel()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CModel::Init(const char* pModelName)
{
	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルマネージャークラスの取得
	CModelManager* pModelManager = CManager::GetModel();

	// モデルの番号の登録
	m_nModelIdx = pModelManager->Register(pModelName);

	if (m_nModelIdx == -1)
	{
		return E_FAIL;
	}

	// マテリアルの取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);
	LPD3DXBUFFER pBuffMat = pModel->GetBuffMat(m_nModelIdx);

	D3DXMATERIAL* pMat;//マテリアルへのポインタ

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	// マテリアルの数分メモリの確保
	m_pTextureIdx = new int[dwNumMat];

	for (int nCnt = 0; nCnt < (int)dwNumMat; nCnt++)
	{
		// ファイル名を使用してテクスチャを読み込む
		m_pTextureIdx[nCnt] = pTexture->RegisterX(pMat[nCnt].pTextureFilename);
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CModel::Uninit(void)
{
	if (m_pTextureIdx != nullptr)
	{
		delete[]m_pTextureIdx;
		m_pTextureIdx = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CModel::Update(void)
{
}

//===================================================
// 描画処理
//===================================================
void CModel::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal,mtxParent;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	D3DXVECTOR3 rot = m_rot + m_offrot;
	D3DXVECTOR3 pos = m_pos + m_offpos;

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pParent != nullptr)
	{ // 親が存在している
		// 親モデルのマトリックスの取得
		mtxParent = m_pParent->GetMatrixWorld();
	}
	else
	{
		// ワールドマトリックスの取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//保存していたマテリアルを元に戻す
		pDevice->SetMaterial(&matDef);

		return;
	}

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// マテリアルの総数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
		//モデル(パーツ)の描画
		pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを元に戻す
	pDevice->SetMaterial(&matDef);
}

//===================================================
// 描画処理
//===================================================
void CModel::Draw(const float fAlv)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal, mtxParent;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	D3DXVECTOR3 rot = m_rot + m_offrot;
	D3DXVECTOR3 pos = m_pos + m_offpos;

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pParent != nullptr)
	{ // 親が存在している
		// 親モデルのマトリックスの取得
		mtxParent = m_pParent->GetMatrixWorld();
	}
	else
	{
		// ワールドマトリックスの取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//保存していたマテリアルを元に戻す
		pDevice->SetMaterial(&matDef);

		return;
	}

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// マテリアルの総数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		D3DXMATERIAL Mat = pMat[nCntMat];

		Mat.MatD3D.Diffuse.a = fAlv;

		//マテリアルの設定
		pDevice->SetMaterial(&Mat.MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
		//モデル(パーツ)の描画
		pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを元に戻す
	pDevice->SetMaterial(&matDef);
}

//===================================================
// 影の描画処理
//===================================================
void CModel::DrawShadow(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// ライトの方向
	D3DXVECTOR4 lightDir = D3DXVECTOR4(-0.707f, -0.707f, -0.707f, 0.0f);

	// 影を落とす地面
	D3DXPLANE plane = D3DXPLANE(0, -1.0f, 0, 0);

	// 影用のマトリックスを作成
	D3DXMATRIX mtxShadow, mtxShadowWorld;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	// 影用のマトリックスの作成
	D3DXMatrixShadow(&mtxShadow, &lightDir, &plane);

	// 影のマトリックスと自分のマトリックスを掛け合わせる
	D3DXMatrixMultiply(&mtxShadowWorld, &m_mtxWorld, &mtxShadow);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadowWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// 半透明の黒マテリアルをセット
	D3DMATERIAL9 matShadow = {};

	// 影の色の設定
	matShadow.Diffuse.r = 0.0f;
	matShadow.Diffuse.g = 0.0f;
	matShadow.Diffuse.b = 0.0f;
	matShadow.Diffuse.a = 1.0f;

	// モデルの取得
	CModelManager* pModel = CManager::GetModel();

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	// メッシュがnullptrだったら関数を抜ける
	if (pMesh == nullptr) return;

	// マテリアルの数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// 影の描画
	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		// 影のマテリアルの設定
		pDevice->SetMaterial(&matShadow);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// 影の描画
		pMesh->DrawSubset(nCntMat);
	}

	// マテリアルをもとに戻す
	pDevice->SetMaterial(&matDef);
}

//===================================================
// ポーズのセーブ
//===================================================
void CModel::SavePose(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	m_Pose.pos = pos;
	m_Pose.rot = rot;
}

//===================================================
// ポーズの設定
//===================================================
void CModel::SetPose(void)
{
	m_pos = m_Pose.pos;
	m_rot = m_Pose.rot;
}

//===================================================
// モデルのマトリックスの取得
//===================================================
D3DXMATRIX CModel::GetModelMatrix(void) const
{
	return m_mtxWorld;
}

//===================================================
// 生成処理
//===================================================
CModel* CModel::Create(const char *pModelName)
{
	CModel* pModel = nullptr;

	// モデルの生成
	pModel = new CModel;

	if (pModel == nullptr) return nullptr;

	pModel->Init(pModelName);

	return pModel;
}

//===================================================
// キャラクターの設定のロード処理
//===================================================
int CModel::LoadCharctorSet(const char* pLoadFileName, CModel** pModel,const int nMaxSize)
{
	// ファイルを開く
	FILE* pFile = fopen(pLoadFileName, "r");
	
	int nNumModel = 0; // モデルの総数
	int nIdx = 0;	   // モデルのインデックス
	int nParent = 0;   // モデルの親

	if (pFile != NULL)
	{
		char aString[MAX_WORD] = {}; // 文字列格納用変数
		char aSkip[MAX_WORD] = {};	 // [=] を飛ばす変数

		while (1)
		{
			// ファイルから文字を読み取る
			int nData = fscanf(pFile, "%s", &aString[0]);

			if (strcmp(aString, "MODEL_FILENAME") == 0)
			{
				// [=]を飛ばす
				nData = fscanf(pFile, "%s", &aSkip[0]);
				nData = fscanf(pFile, "%s", &aString[0]);

				if (nNumModel <= nMaxSize - 1)
				{
					// モデルの生成
					pModel[nNumModel] = CModel::Create(aString);

					nNumModel++;
				}
				else
				{
					MessageBox(NULL, aString, "これ以上読み込めません", MB_OK);
				}
			}
			if (strcmp(aString, "INDEX") == 0)
			{
				// [=]を飛ばす
				nData = fscanf(pFile, "%s", &aSkip[0]);
				nData = fscanf(pFile, "%d", &nIdx);
			}
			else if (strcmp(aString, "PARENT") == 0)
			{
				// [=]を飛ばす
				nData = fscanf(pFile, "%s", &aSkip[0]);
				nData = fscanf(pFile, "%d", &nParent);

				if (nParent != -1)
				{// 親が存在していたら
					// 親のモデルの設定
					pModel[nIdx]->SetParent(pModel[nParent]);
				}
				else
				{// 親が存在していなかったら
					pModel[nIdx]->SetParent(nullptr);
				}
			}
			else if (strcmp(aString, "POS") == 0)
			{
				// [=]を飛ばす
				nData = fscanf(pFile, "%s", &aSkip[0]);
				nData = fscanf(pFile, "%f", &pModel[nIdx]->m_offpos.x);
				nData = fscanf(pFile, "%f", &pModel[nIdx]->m_offpos.y);
				nData = fscanf(pFile, "%f", &pModel[nIdx]->m_offpos.z);
			}
			else if (strcmp(aString, "ROT") == 0)
			{
				// [=]を飛ばす
				nData = fscanf(pFile, "%s", &aSkip[0]);
				nData = fscanf(pFile, "%f", &pModel[nIdx]->m_offrot.x);
				nData = fscanf(pFile, "%f", &pModel[nIdx]->m_offrot.y);
				nData = fscanf(pFile, "%f", &pModel[nIdx]->m_offrot.z);
			}

			if (strcmp(aString, "END_CHARACTERSET") == 0)
			{
				break;
			}
		}
	}
	else
	{
		// メッセージボックス
		MessageBox(NULL, pLoadFileName, "ファイルが開けませんでした", MB_OK);
		return 0;
	}

	// ファイルを閉じる
	fclose(pFile);

	return nNumModel;
}
