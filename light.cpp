//===================================================
//
// ライト [light.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "light.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CLight::CLight()
{
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));
}

//===================================================
// デストラクタ
//===================================================
CLight::~CLight()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CLight::Init(void)
{
	// 値のクリア
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CLight::Uninit(void)
{
}

//===================================================
// 更新処理
//===================================================
void CLight::Update(void)
{

}

//===================================================
// ライトの設定処理
//===================================================
void CLight::SetLight(const D3DLIGHTTYPE type,const D3DXCOLOR Diffuse,const D3DXVECTOR3 dir,const D3DXVECTOR3 pos)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCnt = 0; nCnt < NUM_LIGHT; nCnt++)
	{
		if (m_aLightInfo[nCnt].bUse == false)
		{
			m_aLightInfo[nCnt].aLight.Type = type;
			m_aLightInfo[nCnt].aLight.Diffuse = Diffuse;

			// ライトの方向ベクトル
			D3DXVECTOR3 Direction = dir;

			// 正規化する
			D3DXVec3Normalize(&Direction, &Direction);

			// 方向ベクトルの代入
			m_aLightInfo[nCnt].aLight.Direction = Direction;

			// 位置の設定
			m_aLightInfo[nCnt].aLight.Position = pos;

			m_aLightInfo[nCnt].bUse = true;

			// ライトの設定
			pDevice->SetLight(nCnt, &m_aLightInfo[nCnt].aLight);

			// ライトを有効化
			pDevice->LightEnable(nCnt, TRUE);

			break;
		}
	}
}
