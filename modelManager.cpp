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
#include"LoadManager.h"

//**********************************************
// 静的メンバ変数宣言
//**********************************************
int CModelManager::m_nNumAll = 0;	// モデルの総数

//==============================================
// コンストラクタ
//==============================================
CModelManager::CModelManager()
{

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

	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	for (int nCnt = 0; nCnt < nNumModel; nCnt++)
	{
		if (pFilename == NULL || m_apModelInfo[nCnt].filepath == NULL)
		{
			continue;
		}

		if (strcmp(m_apModelInfo[nCnt].filepath, pFilename) == 0)
		{
			return nCnt;
		}
	}

	if (nIdx == -1)
	{
		// モデルの情報
		ModelInfo info = {};

		////Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(pFilename,
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&info.pBuffMat,
			NULL,
			&info.dwNumMat,
			&info.pMesh)))
		{
			// メッセージボックスの表示
			MessageBox(NULL, pFilename, "モデルが読み込めませんでした", MB_OK | MB_ICONWARNING);

			return -1;
		}

		// 文字列をコピーする
		strncpy(info.filepath, pFilename, sizeof(info.filepath));

		// 大きさの設定
		SetSize(&info);

		// マテリアルの設定
		SetMaterial(&info);

		// 要素の設定
		m_apModelInfo.push_back(info);

		// インデックスの番号を返す
		nIdx = m_nNumAll;

		m_nNumAll++;
	}

	return nIdx;
}

//==============================================
// モデルの情報
//==============================================
CModelManager::ModelInfo CModelManager::GetModelInfo(const int nIdx)
{
	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	if (m_apModelInfo.empty())
	{
		MessageBox(NULL, "モデルが読み込まれていません", "モデルを登録してください", MB_OK);
		return ModelInfo();
	}

	if (nIdx < 0 || nIdx >= nNumModel)
	{
		assert(false && "インデックスオーバーModelInfo");
		return ModelInfo();
	}

	return m_apModelInfo[nIdx];
}

//==============================================
// すべてのxFileのロード処理
//==============================================
HRESULT CModelManager::Load(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ファイルを開く
	fstream file("data/Modellist.txt");
	string line, input;

	// nullじゃなかったら
	if (file.is_open())
	{
		// ロードマネージャーの生成
		unique_ptr<CLoadManager> pLoad = make_unique<CLoadManager>();

		// ファイルを一行ずつ読み取る
		while (getline(file, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			// 文字列が一致していたら
			if (line.find("FILENAME") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				// モデルの名前格納用変数
				string modelName;

				// 数値を代入する
				value_Input >> modelName;

				// モデルの名前を代入
				const char* MODEL_NAME = modelName.c_str();

				// モデルの読み込み
				Register(MODEL_NAME);
			}
		}

		// ファイルを閉じる
		file.close();
		file.clear();
	}
	else
	{
		// メッセージボックス
		MessageBox(NULL, "ファイルが開けません", "modelManager.txt", MB_OK | MB_ICONWARNING);
		return E_FAIL;
	}

	return S_OK;
}

//==============================================
// すべてのxFileの破棄処理
//==============================================
void CModelManager::UnLoad(void)
{
	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	// すべてのモデルのクリア
	for (int nCnt = 0; nCnt < nNumModel; nCnt++)
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

	// 要素のクリア
	m_apModelInfo.clear();
}

//==============================================
// 大きさの設定処理
//==============================================
void CModelManager::SetSize(ModelInfo* pModelInfo)
{
	// 頂点座標の取得
	int nNumVtx;	// 頂点数
	DWORD sizeFVF;  // 頂点フォーマットのサイズ
	BYTE* pVtxBuff; // 頂点バッファへのポインタ

	// 頂点数の取得
	nNumVtx = pModelInfo->pMesh->GetNumVertices();

	// 頂点フォーマットのサイズ取得
	sizeFVF = D3DXGetFVFVertexSize(pModelInfo->pMesh->GetFVF());

	// 頂点バッファのロック
	pModelInfo->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 最大の頂点、最小の頂点
	D3DXVECTOR3 vtxMin = VEC3_NULL, vtxMax = VEC3_NULL;

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		// 頂点座標の代入
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

		// 頂点座標を比較してブロックの最小値,最大値を取得
		if (vtx.x < vtxMin.x)
		{
			vtxMin.x = vtx.x;
		}
		if (vtx.y < vtxMin.y)
		{
			vtxMin.y = vtx.y;
		}
		if (vtx.z < vtxMin.z)
		{
			vtxMin.z = vtx.z;
		}
		if (vtx.x > vtxMax.x)
		{
			vtxMax.x = vtx.x;
		}
		if (vtx.y > vtxMax.y)
		{
			vtxMax.y = vtx.y;
		}
		if (vtx.z > vtxMax.z)
		{
			vtxMax.z = vtx.z;
		}

		// 頂点フォーマットのサイズ分ポインタを進める
		pVtxBuff += sizeFVF;
	}

	// サイズに代入
	pModelInfo->Size.x = vtxMax.x - vtxMin.x;
	pModelInfo->Size.y = vtxMax.y - vtxMin.y;
	pModelInfo->Size.z = vtxMax.z - vtxMin.z;

	// 頂点バッファのアンロック
	pModelInfo->pMesh->UnlockVertexBuffer();
}

//==============================================
// マテリアルの設定処理
//==============================================
void CModelManager::SetMaterial(ModelInfo* pModelInfo)
{
	// マテリアルの取得
	D3DXMATERIAL* pMat;//マテリアルへのポインタ

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModelInfo->pBuffMat->GetBufferPointer();

	// テクスチャマネージャーの取得
	CTextureManager* pTextureManager = CManager::GetTexture();

	for (int nCnt = 0; nCnt < (int)pModelInfo->dwNumMat; nCnt++)
	{
		int nTextureIdx = -1;

		// ファイル名を使用してテクスチャを読み込む
		nTextureIdx = pTextureManager->Register(pMat[nCnt].pTextureFilename);

		pModelInfo->nTextureIdx.push_back(nTextureIdx);
	}
}
