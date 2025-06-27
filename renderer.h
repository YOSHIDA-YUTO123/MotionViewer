//===================================================
//
// �|���S���̕`�揈�� [renderer.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _RENDERER_H_
#define _RENDERER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include "imguimaneger.h"

//***************************************************
// �����_���[�N���X�̒�`
//***************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(const int fps);
	void ResetDevice(void);
	LPDIRECT3DDEVICE9 GetDevice(void);
	HRESULT GetDeviceLost(void);
	void SetReSize(const UINT Width, const UINT Height);
	void onWireFrame();
	void offWireFrame();
	static CImGuiManager *GetImGui(void);
private:
	static CImGuiManager* m_pImGui;			// IMGUI�N���X�ւ̃|�C���^
	UINT m_ResizeWidth, m_ResizeHeight;		// ��ʂ̑傫���ύX���̑傫��
	D3DPRESENT_PARAMETERS m_d3dpp;			// �v���[���e�[�V�����p�����[�^
	LPDIRECT3D9 m_pD3D;						// Directx�f�o�C�X�ւ̃|�C���^
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// Directx�f�o�C�X�ւ̃|�C���^
	bool m_bDeviceLost;						// �f�o�C�X�̃��X�g����
};

#endif