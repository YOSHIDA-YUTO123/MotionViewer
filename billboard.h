//===================================================
//
// ビルボードの描画 [billboard.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 4);
	virtual ~CObjectBillboard();

	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const D3DXVECTOR3 Size) { m_Size = Size; }
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName);
	void UpdateVertexPos(const D3DXVECTOR3 pos);
	void SetTextureVtx(const D3DXVECTOR2 tex, const D3DXVECTOR2 offtex);

	static CObjectBillboard* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const char* pTextureName);
private:
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_Size;						// 大きさ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	int m_nTextureIdx;						// テクスチャのインデックス
};

//***************************************************
// ビルボード(アニメーション)クラスの定義
//***************************************************
class CBillboardAnimation : public CObjectBillboard
{
public:
	CBillboardAnimation(int nPriority = 4);
	~CBillboardAnimation();

	void SetAnim(const int nAnimSpeed,const int U,const int V);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	float m_fPosTexU;	// テクスチャ座標UV
	float m_fPosTexH;	// テクスチャ座標HV
	int m_nAnimSpeed;	// アニメーションの速さ
	int m_nPatternAnim;	// アニメーションのパターン
	int m_nCounterAnim;	// アニメーションのカウンター
	int m_nU,m_nV;		// テクスチャのUVの分割数
};
#endif