//===================================================
//
// マネージャー [manager.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "manager.h"
#include"renderer.h"
#include "object2D.h"
#include "object3D.h"
#include "objectX.h"
#include "player.h"
#include "meshfield.h"
#include "cylinder.h"
#include "dome.h"

//***************************************************
// 静的メンバ変数の宣言
//***************************************************
CRenderer* CManager::m_pRenderer = nullptr;				// レンダラーへのポインタ
CInputKeyboard *CManager::m_pInputKeyboard = nullptr;	// キーボードへのポインタ
CInputJoypad* CManager::m_pInputJoypad = nullptr;		// パッドへのポインタ
CInputMouse* CManager::m_pInputMouse = nullptr;			// マウスへのポインタ
int CManager::m_nFrameCounter = 0;						// フレームのカウンター
bool CManager::m_bShowDebug = true;						// デバッグ表示をするかしないか
CTextureManager* CManager::m_pTexture = nullptr;		// テクスチャクラスへのポインタ
CCamera* CManager::m_pCamera = nullptr;					// カメラのポインタ
CLight* CManager::m_pLight = nullptr;					// カメラへのポインタ
CModelManager* CManager::m_pModel = nullptr;			// モデルのクラスへのポインタ
CPlayer* CManager::m_pPlayer = nullptr;					// プレイヤークラスへのポインタ
bool CManager::m_bPause = false;						// ポーズ
CMeshField* CManager::m_pMeshField = nullptr;

//===================================================
// コンストラクタ
//===================================================
CManager::CManager()
{
	m_fps = 0;
}
//===================================================
// デストラクタ
//===================================================
CManager::~CManager()
{
}
//===================================================
// 初期化処理
//===================================================
HRESULT CManager::Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow)
{
	// レンダラーを生成
	m_pRenderer = new CRenderer;

	// キーボードを生成
	m_pInputKeyboard = new CInputKeyboard;

	// パッドを生成
	m_pInputJoypad = new CInputJoypad;

	// マウスを生成
	m_pInputMouse = new CInputMouse;

	// 初期化処理
	if (FAILED(m_pRenderer->Init(hWnd, bWindow))) return E_FAIL;

	// キーボードの初期化処理
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd))) return E_FAIL;

	// パッドの初期化処理
	if (FAILED(m_pInputJoypad->Init())) return E_FAIL;

	// マウスの初期化処理
	if (FAILED(m_pInputMouse->Init(hWnd))) return E_FAIL;

	// テクスチャの生成
	m_pTexture = new CTextureManager;

	// すべてのテクスチャの読み込み処理
	m_pTexture->Load();

	// モデルの生成
	m_pModel = new CModelManager;
	m_pModel->Load();

	// カメラの生成
	m_pCamera = new CCamera;
	m_pCamera->Init();

	// ライトの生成
	m_pLight = new CLight;
	m_pLight->Init();
	m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f,1.0f, 1.0f,1.0f), D3DXVECTOR3(-0.3f, -0.56f, 0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
	m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.18f, -0.29f, -0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));

	//m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, WHITE, D3DXVECTOR3(-0.2f, -0.1f, 0.4f), D3DXVECTOR3(0.0f, 500.0f, 0.0f));

	//m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, WHITE, D3DXVECTOR3(-0.2f, -0.8f, -0.4f), D3DXVECTOR3(0.0f, 500.0f, 0.0f));

	//m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, WHITE, D3DXVECTOR3(-0.2f, -0.8f, 0.4f), D3DXVECTOR3(0.0f, 500.0f, 0.0f));

	//m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, WHITE, D3DXVECTOR3(0.2f, -0.8f, -0.4f), D3DXVECTOR3(0.0f, 500.0f, 0.0f));

	// フィールドの設定
	m_pMeshField = CMeshField::Create(VEC3_NULL ,64, 64, D3DXVECTOR2(5500.0f,5500.0f));

	//// シリンダーの生成
	//CMeshCylinder::Create(D3DXVECTOR3(500.0f,0.0f,0.0f), 10, 10,500.0f,500.0f);

	// ドームの生成
	CMeshDome::Create(VEC3_NULL,16,16,10000.0f,1500.0f);
	CMeshDome::Create(VEC3_NULL, 16, 16, -10000.0f, -1500.0f);

	//CObject3D::Create(D3DXVECTOR3(0.0f,250.0f,0.0f), VEC3_NULL, D3DXVECTOR3(500.0f, 500.0f, 500.0f), "data/TEXTURE/field.png");

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create();

	//CObjectX::Create(VEC3_NULL, "data/MODEL/sandbug.x");

	// 結果を返す
	return S_OK;
}
//===================================================
// 終了処理
//===================================================
void CManager::Uninit(void)
{
	if (m_pMeshField != nullptr)
	{
		m_pMeshField->Uninit();
		m_pMeshField = nullptr;
	}

	if (m_pPlayer != nullptr)
	{
		m_pPlayer = nullptr;
	}

	// モデルの破棄
	if (m_pModel != nullptr)
	{
		m_pModel->UnLoad();
		delete m_pModel;
		m_pModel = nullptr;
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		// 終了処理
		m_pCamera->Uninit();

		delete m_pCamera;

		m_pCamera = nullptr;
	}

	// ライトの破棄
	if (m_pLight != nullptr)
	{
		// 終了処理
		m_pLight->Uninit();

		delete m_pLight;

		m_pLight = nullptr;
	}

	// テクスチャの破棄
	if (m_pTexture != nullptr)
	{
		// 終了処理
		m_pTexture->UnLoad();

		delete m_pTexture;

		m_pTexture = nullptr;

	}

	// キーボードの破棄
	if (m_pInputKeyboard != nullptr)
	{
		// キーボードの終了処理
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;

		m_pInputKeyboard = nullptr;
	}

	// パッドの破棄
	if (m_pInputJoypad != nullptr)
	{
		// パッドの終了処理
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;

		m_pInputJoypad = nullptr;
	}

	// パッドの破棄
	if (m_pInputMouse != nullptr)
	{
		// マウスの終了処理
		m_pInputMouse->Uninit();

		delete m_pInputMouse;

		m_pInputMouse = nullptr;
	}

	// レンダラーの破棄
	if (m_pRenderer != nullptr)
	{
		// オブジェクトの終了処理
		m_pRenderer->Uninit();

		delete m_pRenderer;

		m_pRenderer = nullptr;
	}
}
//===================================================
// 更新処理
//===================================================
void CManager::Update(void)
{
	// フレームのカウンターを増やす
	m_nFrameCounter++;

	// 1フレーム経過したら
	if (m_nFrameCounter > FRAME)
	{
		// カウンターをリセット
		m_nFrameCounter = 0;
	}

	EnablePause();

	if (m_bPause == false)
	{
		// 更新処理
		m_pRenderer->Update();
	}

	// キーボードの更新処理
	m_pInputKeyboard->Update();

	// パッドの更新処理
	m_pInputJoypad->Update();

	// マウスの更新処理
	m_pInputMouse->Update();

	// カメラの更新処理
	m_pCamera->Update();

	// ライトの更新処理
	m_pLight->Update();

	if (m_pInputKeyboard->GetTrigger(DIK_F2))
	{
		m_bShowDebug = m_bShowDebug ? false : true;
	}

	// ワイヤーフレームの状態
	static bool bWireFrame = false;

	if (m_pInputKeyboard->GetTrigger(DIK_F6))
	{
		bWireFrame = bWireFrame ? false : true;
	}

	if (bWireFrame == true)
	{
		m_pRenderer->onWireFrame();
	}
	else
	{
		m_pRenderer->offWireFrame();
	}

}
//===================================================
// 描画処理
//===================================================
void CManager::Draw(void)
{
	// 更新処理
	m_pRenderer->Draw(m_fps);

}
//===================================================
// レンダラーの取得処理
//===================================================
CRenderer* CManager::GetRenderer(void)
{
	// レンダラーがNULLだったらNULLを返す
	if (m_pRenderer == nullptr) return nullptr;

	return m_pRenderer;
}
//===================================================
// キーボードの取得処理
//===================================================
CInputKeyboard* CManager::GetInputKeyboard(void)
{
	// NULLだったら
	if (m_pInputKeyboard == nullptr) return nullptr;

	return m_pInputKeyboard;
}
//===================================================
// パッドの取得処理
//===================================================
CInputJoypad* CManager::GetInputJoypad(void)
{
	// NULLだったら
	if (m_pInputJoypad == nullptr) return nullptr;

	return m_pInputJoypad;
}
//===================================================
// マウスの取得処理
//===================================================
CInputMouse* CManager::GetInputMouse(void)
{
	// NULLだったら
	if (m_pInputMouse == nullptr) return nullptr;

	return m_pInputMouse;
}

//===================================================
// フレームのカウンター
//===================================================
int CManager::GetFrameCounter(void)
{
	// 現在のフレームの取得
	return m_nFrameCounter;
}

//===================================================
// テクスチャクラスへのポインタ
//===================================================
CTextureManager* CManager::GetTexture(void)
{
	// NULLだったら
	if (m_pTexture == nullptr) return nullptr;

	return m_pTexture;
}

//===================================================
// カメラの取得
//===================================================
CCamera* CManager::GetCamera(void)
{
	// NULLだったら
	if (m_pCamera == nullptr) return nullptr;

	return m_pCamera;
}

//===================================================
// ライトの取得
//===================================================
CLight* CManager::GetLight(void)
{
	// NULLだったら
	if (m_pLight == nullptr) return nullptr;

	return m_pLight;
}

//===================================================
// モデルクラスの取得
//===================================================
CModelManager* CManager::GetModel(void)
{
	// NULLだったら
	if (m_pModel == nullptr) return nullptr;

	return m_pModel;
}

//===================================================
// プレイヤークラスの取得
//===================================================
CPlayer* CManager::GetPlayer(void)
{
	// NULLだったら
	if (m_pPlayer == nullptr) return nullptr;

	return m_pPlayer;
}

//===================================================
// ポーズの設定
//===================================================
void CManager::EnablePause(void)
{
	if (m_pInputKeyboard->GetTrigger(DIK_P))
	{
		m_bPause = m_bPause ? false : true;
	}
}

//===================================================
// メッシュフィールドの取得の設定
//===================================================
CMeshField* CManager::GetMeshField(void)
{
	// NULLだったら
	if (m_pMeshField == nullptr) return nullptr;

	return m_pMeshField;
}

