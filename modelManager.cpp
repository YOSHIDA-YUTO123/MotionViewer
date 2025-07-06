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
		ModelInfo info;

		// 値をクリアしておく
		memset(&info, NULL, sizeof(info));

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

		// 要素の設定
		m_apModelInfo.push_back(info);

		// インデックスの番号を返す
		nIdx = m_nNumAll;

		m_nNumAll++;
	}

	return nIdx;
}

//==============================================
// メッシュの取得
//==============================================
LPD3DXMESH CModelManager::GetMesh(int nIdx)
{
	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	if (nIdx < 0 || nIdx >= nNumModel)
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
	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	if (nIdx < 0 || nIdx >= nNumModel)
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
	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	if (nIdx < 0 || nIdx >= nNumModel)
	{
		return 0;
	}

	return m_apModelInfo[nIdx].dwNumMat;
}

//==============================================
// 大きさの取得
//==============================================
D3DXVECTOR3 CModelManager::GetSize(int nIdx)
{
	// モデルの数
	int nNumModel = static_cast<int>(m_apModelInfo.size());

	if (nIdx < 0 || nIdx >= nNumModel)
	{
		return VEC3_NULL;
	}

	// 頂点座標の取得
	int nNumVtx;	// 頂点数
	DWORD sizeFVF;  // 頂点フォーマットのサイズ
	BYTE* pVtxBuff; // 頂点バッファへのポインタ

	// 頂点数の取得
	nNumVtx = m_apModelInfo[nIdx].pMesh->GetNumVertices();

	// 頂点フォーマットのサイズ取得
	sizeFVF = D3DXGetFVFVertexSize(m_apModelInfo[nIdx].pMesh->GetFVF());

	// 頂点バッファのロック
	m_apModelInfo[nIdx].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		// 頂点座標の代入
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

		// 頂点座標を比較してブロックの最小値,最大値を取得
		if (vtx.x < m_apModelInfo[nIdx].vtxMin.x)
		{
			m_apModelInfo[nIdx].vtxMin.x = vtx.x;
		}
		if (vtx.y < m_apModelInfo[nIdx].vtxMin.y)
		{
			m_apModelInfo[nIdx].vtxMin.y = vtx.y;
		}
		if (vtx.z < m_apModelInfo[nIdx].vtxMin.z)
		{
			m_apModelInfo[nIdx].vtxMin.z = vtx.z;
		}
		if (vtx.x > m_apModelInfo[nIdx].vtxMax.x)
		{
			m_apModelInfo[nIdx].vtxMax.x = vtx.x;
		}
		if (vtx.y > m_apModelInfo[nIdx].vtxMax.y)
		{
			m_apModelInfo[nIdx].vtxMax.y = vtx.y;
		}
		if (vtx.z > m_apModelInfo[nIdx].vtxMax.z)
		{
			m_apModelInfo[nIdx].vtxMax.z = vtx.z;
		}

		// 頂点フォーマットのサイズ分ポインタを進める
		pVtxBuff += sizeFVF;
	}

	// 大きさ
	D3DXVECTOR3 Size = VEC3_NULL;

	// サイズに代入
	Size.x = m_apModelInfo[nIdx].vtxMax.x - m_apModelInfo[nIdx].vtxMin.x;
	Size.y = m_apModelInfo[nIdx].vtxMax.y - m_apModelInfo[nIdx].vtxMin.y;
	Size.z = m_apModelInfo[nIdx].vtxMax.z - m_apModelInfo[nIdx].vtxMin.z;

	// 頂点バッファのアンロック
	m_apModelInfo[nIdx].pMesh->UnlockVertexBuffer();

	return Size;
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

				// モデルの情報
				ModelInfo info;

				// 値をクリアしておく
				memset(&info, NULL, sizeof(info));

				////Xファイルの読み込み
				if (FAILED(D3DXLoadMeshFromX(MODEL_NAME,
					D3DXMESH_SYSTEMMEM,
					pDevice,
					NULL,
					&info.pBuffMat,
					NULL,
					&info.dwNumMat,
					&info.pMesh)))
				{
					// メッセージボックスの表示
					MessageBox(NULL, MODEL_NAME, "モデルが読み込めませんでした", MB_OK | MB_ICONWARNING);

					return E_FAIL;
				}

				// 文字列をコピーする
				strncpy(info.filepath, MODEL_NAME, sizeof(info.filepath));

				// 要素の設定
				m_apModelInfo.push_back(info);

				m_nNumAll++;
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
