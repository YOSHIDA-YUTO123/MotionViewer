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
#include<stdio.h>
#include"LoadManager.h"

//*******************************************
// 静的メンバ変数宣言
//*******************************************
int CTextureManager::m_nNumAll = 0;	// テクスチャの総数

using namespace std; // 名前空間stdを使用

//===========================================
// コンストラクタ
//===========================================
CTextureManager::CTextureManager()
{
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
	int nIdx = -1;

	// テクスチャの数
	int nNumTexture = static_cast<int>(m_apTextureInfo.size());

	for (int nCnt = 0; nCnt < nNumTexture; nCnt++)
	{
		// どちらかがnullだったら
		if (m_apTextureInfo[nCnt].filepath == NULL || pFilename == NULL)
		{
			continue;
		}

		// 文字列が一致していたら
		if (strcmp(m_apTextureInfo[nCnt].filepath, pFilename) == 0)
		{
			return nCnt;
		}
	}

	// 指定されたテクスチャが登録されていなかったら
	if (nIdx == -1)
	{
		// テクスチャの情報
		TextureInfo info;

		// 値をクリアしておく
		memset(&info, NULL, sizeof(info));

		// テクスチャの読み込み
		if (FAILED(D3DXCreateTextureFromFile(pDevice,
			pFilename,
			&info.pTexture)))
		{
			return -1;
		}

		// 文字列のコピー
		strncpy(info.filepath, pFilename, sizeof(info.filepath));

		// 要素の設定
		m_apTextureInfo.push_back(info);

		// インデックス番号を返す
		nIdx = m_nNumAll;

		// テクスチャの総数
		m_nNumAll++;
	}

	return nIdx;
}

//===========================================
// テクスチャの取得処理
//===========================================
LPDIRECT3DTEXTURE9 CTextureManager::GetAdress(int nIdx)
{
	// テクスチャの数
	int nNumTexture = static_cast<int>(m_apTextureInfo.size());

	// 範囲外だったら
	if (nIdx < 0 || nIdx >= nNumTexture)
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
	fstream file("data/texture.txt");
	string line,input;

	// nullじゃなかったら
	if (file.is_open())
	{
		// ロードマネージャーの生成
		unique_ptr<CLoadManager> pLoad = make_unique<CLoadManager>();

		// ファイルを一行ずつ読み取る
		while (getline(file,line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			// 文字列が一致していたら
			if (line.find("TEXTURE_NAME") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				// モデルの名前格納用変数
				string fileName;

				// 数値を代入する
				value_Input >> fileName;

				// モデルの名前を代入
				const char* FILE_NAME = fileName.c_str();

				// テクスチャの情報
				TextureInfo info;

				// 値をクリアしておく
				memset(&info, NULL, sizeof(info));

				// 文字列のコピー
				strncpy(info.filepath, FILE_NAME, sizeof(info.filepath));

				// テクスチャの読み込み
				if (FAILED(D3DXCreateTextureFromFile(pDevice,
					FILE_NAME,
					&info.pTexture)))
				{
					MessageBox(NULL, FILE_NAME, "テクスチャが読み込めません", MB_OK | MB_ICONWARNING);
				}

				// 総数を増やす
				m_nNumAll++;

				// 要素の設定
				m_apTextureInfo.push_back(info);
			}
		}

		// ファイルを閉じる
		file.close();
		file.clear();
	}
	else
	{
		MessageBox(NULL, "ファイルが開けません", "textureManager.cpp", MB_OK);
		return S_OK;
	}

	return S_OK;
}

//===========================================
// すべてのテクスチャの解放処理
//===========================================
void CTextureManager::UnLoad(void)
{
	// テクスチャの数
	int nNumTexture = static_cast<int>(m_apTextureInfo.size());

	// すべてのテクスチャ分回す
	for (int nCnt = 0; nCnt < nNumTexture; nCnt++)
	{
		// テクスチャの破棄
		if (m_apTextureInfo[nCnt].pTexture != nullptr)
		{
			m_apTextureInfo[nCnt].pTexture->Release();
			m_apTextureInfo[nCnt].pTexture = nullptr;
		}
	}

	// 要素のクリア
	m_apTextureInfo.clear();
}
