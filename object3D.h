//===================================================
//
// 3Dオブジェクトの描画 [object3D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CObject3D : public CObject
{
public:
	CObject3D();
	~CObject3D();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDraw(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }
	D3DXVECTOR3 GetSize(void) const { return m_Size; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetSize(const D3DXVECTOR3 size) { m_Size = size; }
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void SetTextureID(const char* pTextureName);
	void UpdatePosition(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size);
	void UpdateCol(const D3DXCOLOR col);

	static CObject3D* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot,const D3DXVECTOR3 size,const char *pTextureName);
private:
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR3 m_Size;						// 大きさ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	int m_nTextureIdx;						// テクスチャのインデックス
};
#endif