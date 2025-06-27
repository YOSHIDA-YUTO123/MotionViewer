//================================================
//
// メッシュ [mesh.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _MESH_H_
#define _MESH_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include"object.h"

//************************************************
// メッシュクラスの定義
//************************************************
class CMesh : public CObject 
{
public:
	CMesh();
	virtual ~CMesh();
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	virtual void Draw(void);
	virtual void SetUpDraw(void);

	int GetSegX(void) const { return m_nSegmentX; }
	int GetSegZ(void) const { return m_nSegmentZ; }

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 NormalizeNormal(const int nIdx);
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void UpdatePosition(const D3DXVECTOR3 speed) { m_pos += speed; }
	void UpdateRotation(const D3DXVECTOR3 speed) { m_rot += speed; }

	void SetVtxBuffer(const D3DXVECTOR3 pos, const int nIdx,const D3DXVECTOR2 tex, const D3DXVECTOR3 nor = { 0.0f,1.0f,0.0f }, const D3DXCOLOR col = WHITE);
	void SetIndexBuffer(const WORD Idx, const int nCnt);
	void SetTextureID(const char* pTextureName = NULL);
	void SetVtxElement(const int vertex,const int polygon,const int index);
	void SetSegment(const int nSegX, const int nSegZ);

	D3DXVECTOR3 GetVtxPos(const int nIdx);
	int GetIndex(const int nIdx);
	void SetVtxPos(const D3DXVECTOR3 pos, const int nIdx);
	void SetNormal(const D3DXVECTOR3 nor, const int nIdx);
	void SetColor(const D3DXCOLOR col, const int nIdx);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffMesh; // 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffMesh;  // インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	int m_nSegmentX;						// 横の分割数
	int m_nSegmentZ;						// 縦の分割数
	int m_nIdexTexture;						// テクスチャのインデックス
	int m_nNumVtx;							// 頂点数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIndex;						// インデックスバッファ
};
#endif