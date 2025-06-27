//==============================================
//
// テクスチャマネージャー [textureManager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

//**********************************************
// インクルードファイル
//**********************************************
#include"main.h"
#include "object.h"

//**********************************************
// マクロ定義
//**********************************************
#define MAX_TEXTURE (128) // テクスチャの最大数

//**********************************************
// テクスチャのマネージャークラスの定義
//**********************************************
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	int Register(const char* pFilename);
	int RegisterX(const char* pFilename);
	LPDIRECT3DTEXTURE9 GetAdress(int nIdx);
	HRESULT Load(void);
	void UnLoad(void);
private:

	// テクスチャの情報の定義
	struct TextureInfo
	{
		LPDIRECT3DTEXTURE9 pTexture;			 // テクスチャへのポインタ
		char filepath[MAX_WORD];				 // ファイルパス
	}; 

	TextureInfo m_apTextureInfo[MAX_TEXTURE];	 // テクスチャの情報
	static int m_nNumAll;						 // テクスチャの番号
};

#endif