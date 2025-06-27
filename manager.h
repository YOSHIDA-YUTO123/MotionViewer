//===================================================
//
// マネージャー [manager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MANAGER_H_
#define _MANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "main.h"
#include "input.h"
#include "textureManager.h"
#include"camera.h"
#include"light.h"
#include"modelManager.h"
#include"player.h"
#include"meshfield.h"

//***************************************************
// マネージャークラスの定義
//***************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CRenderer* GetRenderer(void);
	static CInputKeyboard* GetInputKeyboard(void);
	static CInputJoypad* GetInputJoypad(void);
	static CInputMouse* GetInputMouse(void);
	void SetFps(const int fps) { m_fps = fps; }
	static int GetFrameCounter(void);
	static bool GetShowDebug(void) { return m_bShowDebug; }
	static CTextureManager* GetTexture(void);
	static CCamera* GetCamera(void);
	static CLight* GetLight(void);
	static CModelManager* GetModel(void);
	static CPlayer* GetPlayer(void);
	static void EnablePause(void);
	static void SetPause(void) { m_bPause = true; }
	static CMeshField* GetMeshField(void);
private:
	static CRenderer* m_pRenderer;				// レンダラーのポインタ
	static CInputKeyboard* m_pInputKeyboard;	// キーボードのポインタ
	static CInputJoypad* m_pInputJoypad;		// パッドのポインタ
	static CInputMouse* m_pInputMouse;			// マウスのポインタ
	int m_fps;									// fps格納用変数
	static int m_nFrameCounter;					// フレームカウンター
	static bool m_bShowDebug;					// デバッグ表示をするかしないか
	static CTextureManager* m_pTexture;			// テクスチャクラスへのポインタ	
	static CCamera* m_pCamera;					// カメラのポインタ
	static CLight* m_pLight;					// ライトへのポインタ
	static CModelManager* m_pModel;				// モデルクラスへのポインタ
	static CPlayer* m_pPlayer;					// プレイヤークラスへのポインタ
	static bool m_bPause;						// ポーズ
	static CMeshField* m_pMeshField;			// メッシュフィールドへのポインタ
};
#endif