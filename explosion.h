//===================================================
//
// 爆発,煙の描画 [explosion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"billboard.h"

//***************************************************
// 爆発、煙クラスの定義
//***************************************************
class CExplosion : public CBillboardAnimation
{
public:
	CExplosion(int nPriority = 4);
	~CExplosion();

	static CExplosion* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	D3DXCOLOR m_col; // 色
};

//***************************************************
// 瓦礫の煙クラスの定義
//***************************************************
class CDustSmoke : public CObjectBillboard
{
public:
	CDustSmoke(int nPriority = 4);
	~CDustSmoke();

	static CDustSmoke* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	D3DXCOLOR m_col;	// 色
	D3DXVECTOR3 m_move;	// 移動量
};
#endif