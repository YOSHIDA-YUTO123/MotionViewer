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
#include "object.h"
#include <vector>

//**********************************************
// モデルのマネージャークラスの定義
//**********************************************
class CModelManager
{
public:
	// モデルの情報の定義
	struct ModelInfo
	{
		D3DXVECTOR3 Size;				 // 最大、最小の頂点
		LPD3DXMESH pMesh;				 // メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;			 // マテリアルへのポインタ
		DWORD dwNumMat;					 // マテリアルの数
		std::vector<int> nTextureIdx;	 // マテリアルの数分
		char filepath[MAX_WORD];		 // ファイルパス
	};

	CModelManager();
	~CModelManager();

	int Register(const char* pFilename);
	ModelInfo GetModelInfo(const int nIdx);

	HRESULT Load(void);
	void UnLoad(void);

private:
	void SetSize(ModelInfo* pModelInfo);
	void SetMaterial(ModelInfo* pModelInfo);

	std::vector<ModelInfo> m_apModelInfo; // モデルの情報
	static int m_nNumAll;				  // テクスチャの番号
};

#endif
