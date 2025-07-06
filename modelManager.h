//==============================================
//
// モデルマネージャー [modelManager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

//**********************************************
// インクルードファイル
//**********************************************
#include"main.h"
#include"object.h"
#include<vector>
//**********************************************
// マクロ定義
//**********************************************
//#define MAX_MODEL (256) // モデルの最大数

//**********************************************
// モデルのマネージャークラスの定義
//**********************************************
class CModelManager
{
public:
	CModelManager();
	~CModelManager();

	int Register(const char* pFilename);
	LPD3DXMESH GetMesh(int nIdx);
	LPD3DXBUFFER GetBuffMat(int nIdx);
	DWORD GetNumMat(int nIdx);
	D3DXVECTOR3 GetSize(int nIdx);
	HRESULT Load(void);
	void UnLoad(void);
private:

	// モデルの情報の定義
	struct ModelInfo
	{
		D3DXVECTOR3 vtxMin, vtxMax;		 // 最大、最小の頂点
		LPD3DXMESH pMesh;				 // メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;			 // マテリアルへのポインタ
		DWORD dwNumMat;					 // マテリアルの数
		char filepath[MAX_WORD];		 // ファイルパス
	};

	std::vector<ModelInfo> m_apModelInfo; // モデルの情報
	static int m_nNumAll;				  // テクスチャの番号
};

#endif
