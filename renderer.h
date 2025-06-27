//===================================================
//
// ポリゴンの描画処理 [renderer.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _RENDERER_H_
#define _RENDERER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include "imguimaneger.h"

//***************************************************
// レンダラークラスの定義
//***************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(const int fps);
	void ResetDevice(void);
	LPDIRECT3DDEVICE9 GetDevice(void);
	HRESULT GetDeviceLost(void);
	void SetReSize(const UINT Width, const UINT Height);
	void onWireFrame();
	void offWireFrame();
	static CImGuiManager *GetImGui(void);
private:
	static CImGuiManager* m_pImGui;			// IMGUIクラスへのポインタ
	UINT m_ResizeWidth, m_ResizeHeight;		// 画面の大きさ変更時の大きさ
	D3DPRESENT_PARAMETERS m_d3dpp;			// プレゼンテーションパラメータ
	LPDIRECT3D9 m_pD3D;						// Directxデバイスへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// Directxデバイスへのポインタ
	bool m_bDeviceLost;						// デバイスのロスト判定
};

#endif