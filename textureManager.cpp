//===========================================
//
// テクスチャマネージャー [textureManager.h]
// Author:YUTO YOSHIDA
//
//===========================================

//*******************************************
// インクルードファイル
//*******************************************
#include "textureManager.h"
#include "manager.h"
#include "renderer.h"
#include <cassert>
#include<stdio.h>

//*******************************************
// 静的メンバ変数宣言
//*******************************************
int CTextureManager::m_nNumAll = 0;	// テクスチャの総数

//===========================================
// コンストラクタ
//===========================================
CTextureManager::CTextureManager()
{
	// 値の初期化
	memset(m_apTextureInfo, NULL, sizeof(m_apTextureInfo));
}

//===========================================
// デストラクタ
//===========================================
CTextureManager::~CTextureManager()
{
}

//===========================================
// テクスチャの番号の取得
//===========================================
int CTextureManager::Register(const char* pFilename)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// インデックス
	int nIdx = 0;

	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// どちらかがnullだったら
		if (m_apTextureInfo[nCnt].filepath == NULL || pFilename == NULL)
		{
			continue;
		}

		// 文字列が一致していたら
		if (strcmp(m_apTextureInfo[nCnt].filepath, pFilename) == 0)
		{
			nIdx = nCnt;
			break;
		}

		// 指定されたテクスチャが無かったら
		if (m_nNumAll < nCnt)
		{
			// テクスチャの読み込み
			D3DXCreateTextureFromFile(pDevice,
				pFilename,
				&m_apTextureInfo[nCnt].pTexture);

			m_nNumAll++;

			strncpy(m_apTextureInfo[nCnt].filepath, pFilename, sizeof(m_apTextureInfo[nCnt].filepath));

			nIdx = nCnt;
			break;
		}
	}

	return nIdx;
}

//===========================================
// インデックスの登録
//===========================================
int CTextureManager::RegisterX(const char* pFilename)
{
	int nIdx = -1;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (pFilename == NULL)
	{
		return -1;
	}

	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// どちらかがnullだったら
		if (m_apTextureInfo[nCnt].filepath == NULL || pFilename == NULL)
		{
			continue;
		}

		// 文字列が一致していたら
		if (strcmp(m_apTextureInfo[nCnt].filepath, pFilename) == 0)
		{
			nIdx = nCnt;

			break;
		}

		// 指定されたテクスチャが無かったら
		if (m_nNumAll < nCnt)
		{
			//このファイル名を使用してテクスチャを読み込む
				//テクスチャの読み込み
			D3DXCreateTextureFromFile(pDevice,
				pFilename,
				&m_apTextureInfo[nCnt].pTexture);

			strncpy(m_apTextureInfo[nCnt].filepath, pFilename, sizeof(m_apTextureInfo[nCnt].filepath));

			m_nNumAll++;

			nIdx = nCnt;

			break;
		}
	}

	return nIdx;
}

//===========================================
// テクスチャの取得処理
//===========================================
LPDIRECT3DTEXTURE9 CTextureManager::GetAdress(int nIdx)
{
	// 範囲外だったら
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{
		return nullptr;
	}

	return m_apTextureInfo[nIdx].pTexture;
}

//===========================================
// すべてのテクスチャの読み込み処理
//===========================================
HRESULT CTextureManager::Load(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ファイルを開く
	FILE* pFile = fopen("data/texture.txt", "r");

	// nullじゃなかったら
	if (pFile != NULL)
	{
		// 文字列格納用
		char aString[MAX_WORD] = {};
		char aSkip[MAX_WORD] = {};

		while (1)
		{
			// 1文字ずつ読み取る
			int nData = fscanf(pFile, "%s", &aString);

			// 文字列が一致していたら
			if (strcmp(aString, "TEXTURE_NAME") == 0)
			{
				// [=]を飛ばす
				nData = fscanf(pFile, "%s", &aSkip);
				nData = fscanf(pFile, "%s", &aString);

				// ファイルの名前を代入
				const char* FILE_NAME = aString;

				// 文字列のコピー
				strncpy(m_apTextureInfo[m_nNumAll].filepath, FILE_NAME, sizeof(m_apTextureInfo[m_nNumAll].filepath));

				// テクスチャの読み込み
				if (FAILED(D3DXCreateTextureFromFile(pDevice,
					FILE_NAME,
					&m_apTextureInfo[m_nNumAll].pTexture)))
				{
					MessageBox(NULL, FILE_NAME, "テクスチャが読み込めません", MB_OK | MB_ICONWARNING);
				}

				// 総数を増やす
				m_nNumAll++;
			}

			// ファイルの最後まで行ったら
			if (nData == EOF)
			{
				break;
			}
		}
	}
	else
	{
		MessageBox(NULL, "ファイルが開けません", "textureManager.cpp", MB_OK);
		return S_OK;
	}

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}

//===========================================
// すべてのテクスチャの解放処理
//===========================================
void CTextureManager::UnLoad(void)
{
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// テクスチャの破棄
		if (m_apTextureInfo[nCnt].pTexture != nullptr)
		{
			m_apTextureInfo[nCnt].pTexture->Release();
			m_apTextureInfo[nCnt].pTexture = nullptr;
		}
	}
}
