//===================================================
//
// ライト [light.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _LIGHT_H_
#define _LIGHT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// マクロ定義
//***************************************************
#define NUM_LIGHT (30) // ライトの総数

//***************************************************
// ライトのクラスの定義
//***************************************************
class CLight
{
public:
	CLight();
	~CLight();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetLight(const D3DLIGHTTYPE type, const D3DXCOLOR Diffuse, const D3DXVECTOR3 dir, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetDir(void) const { return m_aLightInfo[0].aLight.Direction; }
private:

	// ライトの情報
	struct LightInfo
	{
		D3DLIGHT9 aLight;	// ライトの構造体
		bool bUse;			// 使用状態
	};

	LightInfo m_aLightInfo[NUM_LIGHT]; // ライトの構造体
};
#endif