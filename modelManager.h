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

//**********************************************
// マクロ定義
//**********************************************
#define MAX_MODEL (256) // モデルの最大数

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

	HRESULT Load(void);
	void UnLoad(void);
private:

	// テクスチャの情報の定義
	struct ModelInfo
	{
		LPD3DXMESH pMesh;				 // メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;			 // マテリアルへのポインタ
		DWORD dwNumMat;					 // マテリアルの数
		char filepath[MAX_WORD];		 // ファイルパス
	};

	ModelInfo m_apModelInfo[MAX_MODEL];	 // モデルの情報
	static int m_nNumAll;				 // テクスチャの番号
};

#endif
