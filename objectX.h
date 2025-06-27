//===================================================
//
// オブジェクトXファイル [objectX.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECTX_H_
#define _OBJECTX_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// マクロ定義
//***************************************************
#define MAX_MAT (256) // マテリアルの数

//***************************************************
// オブジェクトXファイルのクラスの定義
//***************************************************
class CObjectX : public CObject
{
public:
	CObjectX(int nPriority = 2);
	~CObjectX();

	static CObjectX* Create(const D3DXVECTOR3 pos, const char* pModelName);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawShadow(void);

	HRESULT LoadModel(const char* pXFileName);
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }

private:
	D3DXVECTOR3 m_pos;		// 位置(オフセット)
	D3DXVECTOR3 m_rot;		// 向き
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	int m_nModelIdx;		// モデルのインデックス
	int *m_pTextureIdx;		// テクスチャのインデックス
};
#endif
