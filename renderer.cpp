//===================================================
//
// �|���S���̕`�揈�� [renderer.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "renderer.h"
#include"object.h"
#include "object2D.h"
#include "manager.h"
#include "imgui_impl_win32.h" 

////***************************************************
//// �ÓI�����o�ϐ��̐錾
////***************************************************
//CDebugProc* CRenderer::m_pDebug = NULL;		// �f�o�b�O�I�u�W�F�N�g�ւ̃|�C���^
CImGuiManager* CRenderer::m_pImGui = nullptr;	// ImGui�ւ̃|�C���^

//===================================================
// �R���X�g���N�^
//===================================================
CRenderer::CRenderer()
{
	m_pD3D = nullptr;			// Directx�̃f�o�C�X�̏�����
	m_pD3DDevice = nullptr;		// Directx�̃f�o�C�X�̏�����
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	m_bDeviceLost = false;
	m_ResizeHeight = 0;
	m_ResizeWidth = 0;
}

//===================================================
// �f�X�g���N�^
//===================================================
CRenderer::~CRenderer()
{

}

//===================================================
// ����������
//===================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;		 // �f�B�X�v���C���[�h

	// Direct3D�I�u�W�F�N�g�̐���
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// ���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp)); // �p�����[�^�̃[���N���A

	m_d3dpp.BackBufferWidth = SCREEN_WIDTH;							// �Q�[����ʃT�C�Y(��)
	m_d3dpp.BackBufferHeight = SCREEN_HEIGHT;							// �Q�[����ʃT�C�Y(����)
	m_d3dpp.BackBufferFormat = d3ddm.Format;							// �o�b�N�o�b�t�@�̌`��
	m_d3dpp.BackBufferCount = 1;										// �o�b�N�o�b�t�@�̐�
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						// �o�b�N�o�b�t�@�̐؂�ւ�(�f���M���ɓ���)
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;						// �f�v�X�o�b�t�@�Ƃ���16bit���g��
	m_d3dpp.Windowed = bWindow;										// �E�C���h�E���[�h
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// ���t���b�V�����[�g
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		// �C���^�[�o��

	// Direct3D�f�o�C�X�̐����`�揈���ƒ��_�������n�[�h�E�F�A�ōs��
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

	// �����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �T���v���[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// �e�N�X�`���X�e�[�W�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// ImGui�}�l�[�W���̐���
	m_pImGui = new CImGuiManager;
	m_pImGui->Init(hWnd, m_pD3DDevice);

	//// ����������
	//m_pDebug->Init();

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CRenderer::Uninit(void)
{
	// ImGui�̔j��
	if (m_pImGui != nullptr)
	{
		m_pImGui->Uninit();
		delete m_pImGui;
		m_pImGui = nullptr;
	}

	//// �I������
	//m_pDebug->Uninit();

	// ���ׂẴI�u�W�F�N�g�̔j��
	CObject::ReleaseAll();

	// Drectx�f�o�C�X�̔j��
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Drectx�f�o�C�X�̔j��
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

//===================================================
// �X�V����
//===================================================
void CRenderer::Update(void)
{
	// Imgui�̃t���[���̕`��J�n
	m_pImGui->NewFrame();

	// ���ׂẴI�u�W�F�N�g�̍X�V����
	CObject::UpdateAll();

	// �t���[���̏I��
	m_pImGui->EndFrame();
}
//===================================================
// �`�揈��
//===================================================
void CRenderer::Draw(const int fps)
{
	// ��ʃN���A(�o�b�N�o�b�t�@&Z�o�b�t�@�̃N���A)
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 255, 255, 255), 1.0f, 0);

	// �`��J�n
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{//�`��J�n�����������ꍇ
		
		// ���ׂẴI�u�W�F�N�g�̕`�揈��
		CObject::DrawAll();

		//CDebugProc::Print("FPS = %d\n", fps);

		//CDebugProc::Print("�f�o�b�O ��\��      : [ F2 ]\n");

		//// �����̕\��
		//CDebugProc::Print("�I�u�W�F�N�g�̑��� : �ő吔 : [ %d ]\n",MAX_OBJECT);

		//for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
		//{
		//	int nNumAll = CObject::GetNumObject(nCntPriority);

		//	// �����̕\��
		//	CDebugProc::Print("[ Priority = %d : Numobj = %d]\n",nCntPriority,nNumAll);
		//}

		//CDebugProc::Print("���C���[�t���[�� : [ F6 ]");

		//CDebugProc::Draw();

		//CDebugProc::Reset();

		// ImGui�̕`��I��
		m_pImGui->EndRenderer();

		// �`��I��
		m_pD3DDevice->EndScene();
	}

	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	HRESULT hr = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	if (hr == D3DERR_DEVICELOST)
	{
		m_bDeviceLost = true;
	}
}

//===================================================
// �f�o�C�X�̃��Z�b�g
//===================================================
void CRenderer::ResetDevice(void)
{
	// ���C�g�̎擾
	CLight *pLight = CManager::GetLight();

	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = m_pD3DDevice->Reset(&m_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();

	// �����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �T���v���[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// �e�N�X�`���X�e�[�W�`���[�g�̐ݒ�
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	pLight->Init();
	pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.3f, -0.56f, 0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
	pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.18f, -0.29f, -0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
}

//===================================================
// �f�o�C�X�̎擾����
//===================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void)
{
	// �f�o�C�X�̃|�C���^��Ԃ�
	return m_pD3DDevice;
}

//===================================================
// �f�o�C�X�̃��X�g����
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

	// �o�b�N�o�b�t�@��ύX��̑傫���ɒ���
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
// ��ʂ̑傫���ύX���̑傫���̐ݒ菈��
//===================================================
void CRenderer::SetReSize(const UINT Width, const UINT Height)
{
	m_ResizeWidth = Width;
	m_ResizeHeight = Height;
}

//===================================================
// ���C���[�t���[���I��
//===================================================
void CRenderer::onWireFrame()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//===================================================
// ���C���[�t���[���I�t
//===================================================
void CRenderer::offWireFrame()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//===================================================
// ImGui�}�l�[�W���[�̎擾
//===================================================
CImGuiManager *CRenderer::GetImGui(void)
{
	if (m_pImGui == nullptr) return nullptr;

	return m_pImGui;
}
