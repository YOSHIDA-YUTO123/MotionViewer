//=============================================================
//
//　Imguimaneger[Imguimaneger.cpp]
//　Author:YUTO YOSHIDA
//
//=============================================================

//*************************************************************
//インクルードファイル
//*************************************************************
#include "imguimaneger.h"
#include "imgui_internal.h"

using namespace ImGui; // 名前空間imguiを使用

//=============================================================
// コンストラクタ
//=============================================================
CImGuiManager::CImGuiManager()
{
}

//=============================================================
// デストラクタ
//=============================================================
CImGuiManager::~CImGuiManager()
{
}

//=============================================================
// 初期化処理
//=============================================================
void CImGuiManager::Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

   // フォント設定（デフォルトのフォントとして Consolas を使用）
	ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/meiryo.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	io.FontDefault = myFont;
	io.FontGlobalScale = 1.0f;
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 1.0f;      // 角丸
	//style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.4f, 0.f, 1.0f); // 好きな色

	//style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);     // 文字色
	//style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 1); // 背景色
	//style.Colors[ImGuiCol_Button] = ImVec4(0.4, 0.7, 0.2, 1); // ボタン
	//style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5, 0.8, 0.3, 1); // ホバー
	//style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3, 0.6, 0.2, 1); // 押されたとき
	//style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2, 0.2, 0.2, 1); // 入力欄の背景
	//style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0, 1.0, 1.0, 1); // スライダーつまみ
	style.Colors[ImGuiCol_Header] = ImVec4(0.4f, 0.4f, 0.9f, 1.0f); // TreeNodeヘッダー

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
}

//=============================================================
// 終了処理
//=============================================================
void CImGuiManager::Uninit(void)
{
	// Cleanup
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//=============================================================
// 位置の設定処理
//=============================================================
void CImGuiManager::SetPosition(ImVec2 pos)
{
	ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
}

//=============================================================
// 大きさの設定処理
//=============================================================
void CImGuiManager::SetSize(ImVec2 Size)
{
	ImGui::SetNextWindowSize(Size, ImGuiCond_Once);
}

//=============================================================
// ウィンドウの描画開始
//=============================================================
void CImGuiManager::Start(const char* WindowName, TYPE type)
{
	if (WindowName != NULL)
	{
		switch (type)
		{
		case TYPE_DEFOULT:
			ImGui::Begin(WindowName);
			break;
		case TYPE_NOMOVE:
			ImGui::Begin(WindowName, NULL, ImGuiWindowFlags_NoMove);
			break;
		case TYPE_NOMOVESIZE:
			ImGui::Begin(WindowName, NULL, ImGuiWindowFlags_NoResize);
			break;
		case TYPE_NOMOVEANDSIZE:
			ImGui::Begin(WindowName, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			break;
		default:
			break;
		}
	}
}

//=============================================================
// 描画の終了処理
//=============================================================
void CImGuiManager::EndRenderer(void)
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

//=============================================================
// ウィンドウのフレームの作成処理
//=============================================================
void CImGuiManager::NewFrame(void)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
