//===================================================
//
// ツールバー [toolbar.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "toolbar.h"

//***************************************************
// コンストラクタ
//***************************************************
CToolbar::CToolbar()
{
}

//***************************************************
// デストラクタ
//***************************************************
CToolbar::~CToolbar()
{

}

//***************************************************
// ツールバーの作成処理
//***************************************************
void CToolbar::CreateToolbar(HWND hWnd,HINSTANCE hInstance)
{
	HWND hToolbar = CreateWindowEx(
		0,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP | CCS_NORESIZE | CCS_NODIVIDER | TBSTYLE_LIST,
		0,
		0,
		0,
		0,
		hWnd,
		(HMENU)TOOLBAR_ID,
		hInstance,
		NULL);

	// 1. イメージリストを作成
	HIMAGELIST hImageList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 4, 1);

	// 2. アイコンをロードしてイメージリストに追加
	HICON hIconOpen = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(TOOLBAR_ID), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);
	ImageList_AddIcon(hImageList, hIconOpen);

	// 3. ツールバーにイメージリストをセット
	SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

	// ツールバーのボタン
	TBBUTTON tbb[4] = {};

	const char *szButtonText = "ファイル\0項目１\0項目２\0項目３\0";
	INT_PTR stringIndex = SendMessage(hToolbar, TB_ADDSTRING, 0, (LPARAM)szButtonText);

	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		tbb[nCnt].iBitmap = I_IMAGENONE;
		tbb[nCnt].idCommand = 100 + nCnt; // WM_COMMAND用のID
		tbb[nCnt].fsState = TBSTATE_ENABLED;
		tbb[nCnt].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE | BTNS_DROPDOWN;
		tbb[nCnt].iString = stringIndex + nCnt;
	}

	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessage(hToolbar, TB_ADDBUTTONS, 4, (LPARAM)&tbb);
	SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hToolbar, TRUE);

	// フォント設定
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hToolbar, WM_SETFONT, (WPARAM)hFont, TRUE);

	// サイズ調整
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	MoveWindow(hToolbar, 0, 0, rcClient.right, 24, TRUE);
}

//***************************************************
// staticコントロールの作成
//***************************************************
HWND CToolbar::CreateStaticCtrl(HWND hWnd, HINSTANCE hInstance)
{
	HWND hSomeStaticCtrl = CreateWindow(
		"STATIC",      // クラス名
		"ここに表示",  // 初期テキスト
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		10, 50, 200, 20,  // 位置とサイズ（x, y, width, height）
		hWnd,          // 親ウィンドウのハンドル
		NULL,
		hInstance,
		NULL);

	return hSomeStaticCtrl;
}
