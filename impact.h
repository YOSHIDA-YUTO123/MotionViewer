//================================================
//
// メッシュインパクト [impact.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _IMPACT_H_
#define _IMPACT_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"

//************************************************
// メッシュフィールドクラスの定義
//************************************************
class CMeshCircle : public CMesh
{
public:
	CMeshCircle();
	~CMeshCircle();

	static CMeshCircle* Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const float InRadius, const float OutRadius,const float speed,const int nLife,const D3DXCOLOR col = WHITE,const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetImpact(const int nSegX, const float InRadius,const float OutRadius);
private:
	D3DXCOLOR m_Incol,m_Outcol;	// 色
	int m_nLife;				// 寿命
	float m_fDecAlv;			// α値の減少スピード
	float m_fSpeed;				// 速さ
	float m_fInRadius;			// 内側の半径
	float m_fOutRadius;			// 外側の半径
};
#endif