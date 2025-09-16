//===================================================
//
// ツールバー [toolbar.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// ツールバークラスの定義
//***************************************************
class CToolbar
{
public:
	static constexpr int TOOLBAR_ID = 123;
	CToolbar();
	~CToolbar();
	static void CreateToolbar(HWND hWnd, HINSTANCE hInstance);
	static HWND CreateStaticCtrl(HWND hWnd, HINSTANCE hInstance);
private:
};

#endif