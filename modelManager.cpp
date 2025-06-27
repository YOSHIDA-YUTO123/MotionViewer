//==============================================
//
// モデルマネージャー [modelManager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// インクルードファイル
//**********************************************
#include "modelManager.h"
#include "manager.h"
#include"renderer.h"
#include<stdio.h>

//**********************************************
// 静的メンバ変数宣言
//**********************************************
int CModelManager::m_nNumAll = 0;	// モデルの総数

//==============================================
// コンストラクタ
//==============================================
CModelManager::CModelManager()
{
	memset(m_apModelInfo, NULL, sizeof(m_apModelInfo));
}

//==============================================
// デストラクタ
//==============================================
CModelManager::~CModelManager()
{
}

//==============================================
// インデックスの登録
//==============================================
int CModelManager::Register(const char* pFilename)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	int nIdx = -1;

	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		if (pFilename == NULL || m_apModelInfo[nCnt].filepath == NULL)
		{
			continue;
		}

		if (strcmp(m_apModelInfo[nCnt].filepath, pFilename) == 0)
		{
			nIdx = nCnt;
			break;
		}

		if (m_nNumAll < nCnt && m_nNumAll <= MAX_MODEL - 1)
		{
			////Xファイルの読み込み
			if (FAILED(D3DXLoadMeshFromX(pFilename,
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&m_apModelInfo[nCnt].pBuffMat,
				NULL,
				&m_apModelInfo[nCnt].dwNumMat,
				&m_apModelInfo[nCnt].pMesh)))
			{
				// メッセージボックスの表示
				MessageBox(NULL, pFilename, "モデルが読み込めませんでした", MB_OK | MB_ICONWARNING);

				return -1;
			}

			// 文字列をコピーする
			strncpy(m_apModelInfo[nCnt].filepath, pFilename, sizeof(m_apModelInfo[nCnt].filepath));

			nIdx = nCnt;

			m_nNumAll++;

			break;
		}
	}
	return nIdx;
}

//==============================================
// メッシュの取得
//==============================================
LPD3DXMESH CModelManager::GetMesh(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return nullptr;
	}

	return m_apModelInfo[nIdx].pMesh;
}

//==============================================
// バッファの取得
//==============================================
LPD3DXBUFFER CModelManager::GetBuffMat(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return nullptr;
	}

	return m_apModelInfo[nIdx].pBuffMat;
}

//==============================================
// マテリアルのの取得
//==============================================
DWORD CModelManager::GetNumMat(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return 0;
	}

	return m_apModelInfo[nIdx].dwNumMat;}

//==============================================
// すべてのxFileのロード処理
//==============================================
HRESULT CModelManager::Load(void)
{
	// ファイルを開く
	FILE* pFile = fopen("data/Modellist.txt", "r");

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ファイルが開けたら
	if (pFile != NULL)
	{
		// 文字列格納用
		char aString[MAX_WORD] = {};
		char aSkip[MAX_WORD] = {};

		while (1)
		{
			// ファイルの文字を読み取る
			int nData = fscanf(pFile, "%s", &aString);

			// 文字列が一致したら
			if (strcmp(aString, "FILENAME") == 0)
			{
				// [=]を読み取る
				nData = fscanf(pFile, "%s", &aSkip);
				nData = fscanf(pFile, "%s", &aString);

				const char* MODEL_NAME = aString;

				// 文字列をコピーする
				strncpy(m_apModelInfo[m_nNumAll].filepath, MODEL_NAME,sizeof(m_apModelInfo[m_nNumAll].filepath));

				////Xファイルの読み込み
				if (FAILED(D3DXLoadMeshFromX(MODEL_NAME,
					D3DXMESH_SYSTEMMEM,
					pDevice,
					NULL,
					&m_apModelInfo[m_nNumAll].pBuffMat,
					NULL,
					&m_apModelInfo[m_nNumAll].dwNumMat,
					&m_apModelInfo[m_nNumAll].pMesh)))
				{
					// メッセージボックスの表示
					MessageBox(NULL, MODEL_NAME, "モデルが読み込めませんでした", MB_OK | MB_ICONWARNING);

					return E_FAIL;
				}

				m_nNumAll++;
			}

			if (nData == EOF)
			{
				break;
			}
		}
	}
	else
	{
		// メッセージボックス
		MessageBox(NULL, "ファイルが開けません", "modelManager.txt", MB_OK | MB_ICONWARNING);
		return E_FAIL;
	}

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}

//==============================================
// すべてのxFileの破棄処理
//==============================================
void CModelManager::UnLoad(void)
{
	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		// メッシュの破棄
		if (m_apModelInfo[nCnt].pMesh != nullptr)
		{
			m_apModelInfo[nCnt].pMesh->Release();
			m_apModelInfo[nCnt].pMesh = nullptr;
		}

		// マテリアルの破棄
		if (m_apModelInfo[nCnt].pBuffMat != nullptr)
		{
			m_apModelInfo[nCnt].pBuffMat->Release();
			m_apModelInfo[nCnt].pBuffMat = nullptr;
		}
	}
}
