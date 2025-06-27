//==================================================
//
// メイン [main.cpp]
// Author:YUTO YOSHIDA
//
//==================================================

//**************************************************
// インクルードファイル
//**************************************************
#include "main.h"
#include<stdlib.h>
#include<crtdbg.h>
#include "manager.h"
#include"renderer.h"

//**************************************************
// プロトタイプ宣言
//**************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==================================================
// メイン関数
//==================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
//#ifdef _DEBUG

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF); //メモリーリーク検知用フラグ

//#endif // _DEBUG

	DWORD dwCurrentTime;					// 現在時刻
	DWORD dwExecLastTime = timeGetTime();	// 最後に処理した時刻
	
	// 分解能を設定
	timeBeginPeriod(1);

	dwCurrentTime = 0;	// 初期化する
	dwExecLastTime = timeGetTime();	// 現在時刻保存

	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				// WNDCLASSEXのメモリサイズ
		CS_CLASSDC,						// ウインドウのスタイル
		WindowProc,						// ウインドウプロシージャ
		0,								// 0にする(通常は使用しない)
		0,								// 0にする(通常は使用しない)
		hInstance,						// インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION),	// タスクバーのアイコン
		LoadCursor(NULL,IDC_ARROW),		// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),		// クライアント領域の色
		NULL,							// メニューバー
		CLASS_NAME,						// ウインドウクラスの名前
		LoadIcon(NULL,IDI_APPLICATION)  // ファイルのアイコン
	};

	HWND hWnd; // ウインドウのハンドル

	MSG msg;

	// ウインドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定サイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウインドウを生成
	hWnd = CreateWindowEx(0,		// 拡張ウィンドウスタイル
		CLASS_NAME,					// ウインドウクラスの名前
		WINDOW_NAME,				// ウインドウの名前
		WS_OVERLAPPEDWINDOW,		// ウインドウのスタイル
		CW_USEDEFAULT,				// ウインドウ左上X座標
		CW_USEDEFAULT,				// ウインドウ左上Y座標
		(rect.right - rect.left),	// ウインドウ左上x座標幅
		(rect.bottom - rect.top),	// ウインドウ左上Y座標高さ
		NULL,						// 親ウインドウのハンドル
		NULL,						// メニューハンドルまたはウインドウID
		hInstance,					// インスタンスハンドル
		NULL);						// ウインドウ作成データ

	// ウインドウの表示
	ShowWindow(hWnd, nCmdShow); // ウインドウの表示状態を設定
	UpdateWindow(hWnd);         // クライアント領域を更新

	DWORD dwFrameCount;  // フレームカウント
	DWORD dwFPSLastTime; // 最後にFPSを計測した時間

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	CManager* pManager = NULL;

	// レンダラーがNULLなら
	if (pManager == NULL)
	{
		// レンダラーの生成
		pManager = new CManager;
	}

	// 初期化処理
	if (FAILED(pManager->Init(hInstance,hWnd, TRUE)))
	{
		return -1;
	}

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{ // windowsの処理
			if (msg.message == WM_QUIT)
			{ // WM_QUITメッセージを受け取ったらループを抜ける
				break;
			}
			else
			{
				TranslateMessage(&msg); // 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);  // ウインドウプロシージャへメッセージを送出
			}
		}
		else
		{
			// DirectXの処理
			// メッセージの設定
			dwCurrentTime = timeGetTime(); // 現在時刻

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{ // 0.5秒経過
				////FPSを計測

				// FPSを計算する
				int fps = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				// FPSの設定
				pManager->SetFps(fps);

				dwFPSLastTime = dwCurrentTime; // FPSを計測した時刻を保存
				dwFrameCount = 0; // フレームカウントをクリア

			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{ // 60分の1秒経過
				dwExecLastTime = dwCurrentTime; // 処理開始の時刻(現在時刻)を保存

				dwFrameCount++; //フレームカウントを加算

				// レンダラーの取得
				CRenderer *pRenderer = pManager->GetRenderer();

				if (pRenderer->GetDeviceLost() == D3DERR_DEVICELOST)
				{
					::Sleep(10);
					continue;
				}

				// 更新処理
				pManager->Update();

				// 描画処理
				pManager->Draw();
			}	
		}	
	}

	// レンダラーの破棄
	if (pManager != nullptr)
	{
		// レンダラーの終了処理
		pManager->Uninit();

		delete pManager;

		pManager = nullptr;
	}

#ifdef _DEBUG

	// メモリーリーク検知
	if (_CrtDumpMemoryLeaks())
	{
		MessageBox(NULL, "メモリリークが検出されました", "エラー", MB_OK | MB_ICONERROR);
		exit(1);
	}

#endif

	// ウインドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

//==================================================
// IMGUIのハンドラー宣言
//==================================================
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//==================================================
// ウインドウプロシージャ
//==================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const RECT rect = { 0,0, SCREEN_WIDTH,SCREEN_HEIGHT }; // ウインドウの領域

	// IMGUIのクリック判定
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	switch (uMsg)
	{
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
			return 0;

		// レンダラーの取得
		CRenderer* pRenderer = CManager::GetRenderer();

		// 現在のウィンドウの大きさを代入
		UINT ResizeWidth = LOWORD(lParam);
		UINT ResizeHeight = HIWORD(lParam);

		if (pRenderer != nullptr)
		{
			// 現在のウィンドウの大きさを設定
			pRenderer->SetReSize(ResizeWidth, ResizeHeight);
		}
	}
		break;
	case WM_DESTROY: // ウインドウの破棄メッセージ
		// WM_QUITへメッセージを送る
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN: // キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:				// [ESC]キーが押された
			int nID = MessageBox(hWnd, "終了しますか?", "終了メッセージ", MB_YESNO);

			if (nID == IDYES)
			{
				DestroyWindow(hWnd);// ウインドウ破棄メッセージ

			}
			else
			{
				return 0;			// 返す
			}
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);    // 既定の処理を返す
}
