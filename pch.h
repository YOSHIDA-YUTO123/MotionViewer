//===================================================
//
// プリコンパイル [pch.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PCH_H_
#define _PCH_H_

//***************************************************
// インクルードファイル
//***************************************************
//#define _CRT_SECURE_NO_WARNINGS		 // scanfの警告対処マクロ
#include<windows.h>
#include"d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800) // ビルド時の警告対処マクロ
#include "dinput.h"
#include"Xinput.h"
#include"xaudio2.h"

//***************************************************
// ライブラリのリンク
//***************************************************
#pragma comment(lib,"d3d9.lib")		// 描画処理に必要
#pragma comment(lib,"d3dx9.lib")	// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")	// DirectXコンポネーション(部品)使用に必要
#pragma comment(lib,"winmm.lib")	// システム時刻取得に必要
#pragma comment(lib,"dinput8.lib")	// 入力処理に必要
#pragma comment(lib,"xinput.lib")	// ジョイパッド処理に必要

//***************************************************
// マクロ定義
//***************************************************
#define CLASS_NAME "WindowClass"	// ウインドウクラスの名前
#define WINDOW_NAME "MotionViewer"	// ウインドウの名前(キャプションに表示)
#define SCREEN_WIDTH (1280)			// ウインドウの幅
#define SCREEN_HEIGHT (720)			// ウインドウの高さ
#define FVF_VERTEX_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FVF_VERTEX_3D (D3DFVF_XYZ | D3DFVF_NORMAL |  D3DFVF_DIFFUSE | D3DFVF_TEX1) // 座標,法線,カラー,テクスチャ

//**********************
// 頂点情報[2D]の構造体
//**********************
typedef struct
{
	D3DXVECTOR3 pos;				// 頂点座標
	float rhw;						// 座標返還係数(1.0fで指定)
	D3DCOLOR col;					// 頂点カラー
	D3DXVECTOR2 tex;				// テクスチャ座標
}VERTEX_2D;

//**********************
// 頂点情報[3D]の構造体
//**********************
typedef struct
{
	D3DXVECTOR3 pos;			// 頂点座標
	D3DXVECTOR3 nor;			// 法線ベクトル
	D3DCOLOR col;				// 頂点カラー
	D3DXVECTOR2 tex;			// テクスチャ座標
}VERTEX_3D;

typedef enum
{
	MODE_TITLE = 0,					// タイトル画面
	MODE_TUTORIAL,					// チュートリアル画面
	MODE_GAME,						// ゲーム画面
	MODE_RESULT,					// リザルト画面
	MODE_RANKING,					// ランキング画面
	MODE_MAX
}MODE;


#endif
