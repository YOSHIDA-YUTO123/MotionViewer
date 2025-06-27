//===================================================
//
// ポリゴンの描画処理 [renderer.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "renderer.h"
#include"object.h"
#include "object2D.h"
#include "manager.h"
#include "imgui_impl_win32.h" 

////***************************************************
//// 静的メンバ変数の宣言
////***************************************************
//CDebugProc* CRenderer::m_pDebug = NULL;		// デバッグオブジェクトへのポインタ
CImGuiManager* CRenderer::m_pImGui = nullptr;	// ImGuiへのポインタ

//===================================================
// コンストラクタ
//===================================================
CRenderer::CRenderer()
{
	m_pD3D = nullptr;			// Directxのデバイスの初期化
	m_pD3DDevice = nullptr;		// Directxのデバイスの初期化
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	m_bDeviceLost = false;
	m_ResizeHeight = 0;
	m_ResizeWidth = 0;
}

//===================================================
// デストラクタ
//===================================================
CRenderer::~CRenderer()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;		 // ディスプレイモード

	// Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp)); // パラメータのゼロクリア

	m_d3dpp.BackBufferWidth = SCREEN_WIDTH;							// ゲーム画面サイズ(幅)
	m_d3dpp.BackBufferHeight = SCREEN_HEIGHT;							// ゲーム画面サイズ(高さ)
	m_d3dpp.BackBufferFormat = d3ddm.Format;							// バックバッファの形式
	m_d3dpp.BackBufferCount = 1;										// バックバッファの数
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						// バックバッファの切り替え(映像信号に同期)
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;						// デプスバッファとして16bitを使う
	m_d3dpp.Windowed = bWindow;										// ウインドウモード
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル

	// Direct3Dデバイスの生成描画処理と頂点処理をハードウェアで行う
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// ImGuiマネージャの生成
	m_pImGui = new CImGuiManager;
	m_pImGui->Init(hWnd, m_pD3DDevice);

	//// 初期化処理
	//m_pDebug->Init();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CRenderer::Uninit(void)
{
	// ImGuiの破棄
	if (m_pImGui != nullptr)
	{
		m_pImGui->Uninit();
		delete m_pImGui;
		m_pImGui = nullptr;
	}

	//// 終了処理
	//m_pDebug->Uninit();

	// すべてのオブジェクトの破棄
	CObject::ReleaseAll();

	// Drectxデバイスの破棄
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Drectxデバイスの破棄
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

//===================================================
// 更新処理
//===================================================
void CRenderer::Update(void)
{
	// Imguiのフレームの描画開始
	m_pImGui->NewFrame();

	// すべてのオブジェクトの更新処理
	CObject::UpdateAll();

	// フレームの終了
	m_pImGui->EndFrame();
}
//===================================================
// 描画処理
//===================================================
void CRenderer::Draw(const int fps)
{
	// 画面クリア(バックバッファ&Zバッファのクリア)
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 255, 255, 255), 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{//描画開始が成功した場合
		
		// すべてのオブジェクトの描画処理
		CObject::DrawAll();

		//CDebugProc::Print("FPS = %d\n", fps);

		//CDebugProc::Print("デバッグ 非表示      : [ F2 ]\n");

		//// 文字の表示
		//CDebugProc::Print("オブジェクトの総数 : 最大数 : [ %d ]\n",MAX_OBJECT);

		//for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
		//{
		//	int nNumAll = CObject::GetNumObject(nCntPriority);

		//	// 文字の表示
		//	CDebugProc::Print("[ Priority = %d : Numobj = %d]\n",nCntPriority,nNumAll);
		//}

		//CDebugProc::Print("ワイヤーフレーム : [ F6 ]");

		//CDebugProc::Draw();

		//CDebugProc::Reset();

		// ImGuiの描画終了
		m_pImGui->EndRenderer();

		// 描画終了
		m_pD3DDevice->EndScene();
	}

	//バックバッファとフロントバッファの入れ替え
	HRESULT hr = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	if (hr == D3DERR_DEVICELOST)
	{
		m_bDeviceLost = true;
	}
}

//===================================================
// デバイスのリセット
//===================================================
void CRenderer::ResetDevice(void)
{
	// ライトの取得
	CLight *pLight = CManager::GetLight();

	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = m_pD3DDevice->Reset(&m_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージチャートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	pLight->Init();
	pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.3f, -0.56f, 0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
	pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.18f, -0.29f, -0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
}

//===================================================
// デバイスの取得処理
//===================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void)
{
	// デバイスのポインタを返す
	return m_pD3DDevice;
}

//===================================================
// デバイスのロスト判定
//===================================================
HRESULT CRenderer::GetDeviceLost(void)
{
	// Handle lost D3D9 device
	if (m_bDeviceLost)
	{
		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
		if (hr == D3DERR_DEVICELOST)
		{
			return D3DERR_DEVICELOST;
		}
		if (hr == D3DERR_DEVICENOTRESET)
		{
			ResetDevice();
			m_bDeviceLost = false;
			return D3DERR_DEVICELOST;
		}
	}

	// バックバッファを変更後の大きさに直す
	if (m_ResizeWidth != 0 && m_ResizeHeight != 0)
	{
		m_d3dpp.BackBufferWidth = m_ResizeWidth;
		m_d3dpp.BackBufferHeight = m_ResizeHeight;
		m_ResizeWidth = m_ResizeHeight = 0;
		ResetDevice();
	}

	return S_OK;
}

//===================================================
// 画面の大きさ変更時の大きさの設定処理
//===================================================
void CRenderer::SetReSize(const UINT Width, const UINT Height)
{
	m_ResizeWidth = Width;
	m_ResizeHeight = Height;
}

//===================================================
// ワイヤーフレームオン
//===================================================
void CRenderer::onWireFrame()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//===================================================
// ワイヤーフレームオフ
//===================================================
void CRenderer::offWireFrame()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//===================================================
// ImGuiマネージャーの取得
//===================================================
CImGuiManager *CRenderer::GetImGui(void)
{
	if (m_pImGui == nullptr) return nullptr;

	return m_pImGui;
}
