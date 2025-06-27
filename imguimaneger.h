//===================================================
//
// imgui�}�l�[�W���[ [imguimanager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _IMGUIMANEGER_H_
#define _IMGUIMANEGER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "main.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

//***************************************************
// IMGUI�}�l�[�W���[�N���X�̒�`
//***************************************************
class CImGuiManager
{
public:
	typedef enum
	{
		TYPE_DEFOULT = 0,
		TYPE_NOMOVE,
		TYPE_NOMOVESIZE,
		TYPE_NOMOVEANDSIZE,
		TYPE_MAX
	}TYPE;

	CImGuiManager();
	~CImGuiManager();

	void Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice);
	void Uninit(void);
	void SetPosition(ImVec2 pos);
	void SetSize(ImVec2 Size);
	void Start(const char* WindowName, TYPE type);
	void EndRenderer(void);
	void NewFrame(void);
	void EndFrame(void) { ImGui::EndFrame(); }
	void End(void) { ImGui::End(); }
private:
};

#endif // !_IMGUIMANEGER_H_