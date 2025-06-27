//=============================================================
//
//�@Imguimaneger[Imguimaneger.cpp]
//�@Author:YUTO YOSHIDA
//
//=============================================================

//*************************************************************
//�C���N���[�h�t�@�C��
//*************************************************************
#include "imguimaneger.h"
#include "imgui_internal.h"

using namespace ImGui; // ���O���imgui���g�p

//=============================================================
// �R���X�g���N�^
//=============================================================
CImGuiManager::CImGuiManager()
{
}

//=============================================================
// �f�X�g���N�^
//=============================================================
CImGuiManager::~CImGuiManager()
{
}

//=============================================================
// ����������
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

   // �t�H���g�ݒ�i�f�t�H���g�̃t�H���g�Ƃ��� Consolas ���g�p�j
	ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/meiryo.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	io.FontDefault = myFont;
	io.FontGlobalScale = 1.0f;
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 1.0f;      // �p��
	//style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.4f, 0.f, 1.0f); // �D���ȐF

	//style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);     // �����F
	//style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 1); // �w�i�F
	//style.Colors[ImGuiCol_Button] = ImVec4(0.4, 0.7, 0.2, 1); // �{�^��
	//style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5, 0.8, 0.3, 1); // �z�o�[
	//style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3, 0.6, 0.2, 1); // �����ꂽ�Ƃ�
	//style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2, 0.2, 0.2, 1); // ���͗��̔w�i
	//style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0, 1.0, 1.0, 1); // �X���C�_�[�܂�
	style.Colors[ImGuiCol_Header] = ImVec4(0.4f, 0.4f, 0.9f, 1.0f); // TreeNode�w�b�_�[

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
}

//=============================================================
// �I������
//=============================================================
void CImGuiManager::Uninit(void)
{
	// Cleanup
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//=============================================================
// �ʒu�̐ݒ菈��
//=============================================================
void CImGuiManager::SetPosition(ImVec2 pos)
{
	ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
}

//=============================================================
// �傫���̐ݒ菈��
//=============================================================
void CImGuiManager::SetSize(ImVec2 Size)
{
	ImGui::SetNextWindowSize(Size, ImGuiCond_Once);
}

//=============================================================
// �E�B���h�E�̕`��J�n
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
// �`��̏I������
//=============================================================
void CImGuiManager::EndRenderer(void)
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

//=============================================================
// �E�B���h�E�̃t���[���̍쐬����
//=============================================================
void CImGuiManager::NewFrame(void)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
