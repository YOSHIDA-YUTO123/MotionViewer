//===================================================
//
// �v���C���[ [player.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "player.h"
#include "input.h"
#include "manager.h"
#include"model.h"
#include"renderer.h"
#include "impact.h"
#include"motion.h"
#include"shadow.h"
#include"math.h"
#include <filesystem>
#include <iomanip>
#include "explosion.h"

using namespace math; // ���O��Ԃ��g�p����
using namespace std;

//***************************************************
// �}�N����`
//***************************************************
#define PLAYER_JUMP_HEIGHT (25.0f)  // �W�����v��
#define MOVE_SPEED (10.5f)			// �ړ����x
#define SHADOW_SIZE (25.0f)		// �e�̑傫��
#define SHADOW_MAX_HEIGHT (500.0f)  // �e��������ő�̍���
#define SHADOW_A_LEVEL (0.9f)       // �e�̃A���t�@�l�̃I�t�Z�b�g

//===================================================
// �R���X�g���N�^
//===================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_move = VEC3_NULL;
	m_rotDest = VEC3_NULL;
	m_fSpeed = NULL;
	m_bJump = true;
	memset(m_apModel, NULL, sizeof(m_apModel));
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_bDash = false;
	m_nModelIndex = 0;
	m_bMotionView = false;
	m_fJumpHeight = NULL;
	m_nSmockModel = NULL;
	m_bViewMode = false;
}

//===================================================
// �f�X�g���N�^
//===================================================
CPlayer::~CPlayer()
{
}

//===================================================
// ����������
//===================================================
HRESULT CPlayer::Init(void)
{
	// �������̃��[�h
	LoadSystemIni();

	m_pShadow = CShadow::Create(VEC3_NULL, 50.0f, 50.0f, WHITE);

	if (m_pMotion != nullptr)
	{
		m_pMotion->Init();
		m_pMotion->Update(&m_apModel[0], m_nNumModel,false);
	}

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CPlayer::Uninit(void)
{
	for (int nCnt = 0; nCnt < NUM_PARTS; nCnt++)
	{
		// ���f���̔j��
		if (m_apModel[nCnt] != nullptr)
		{
			// �I������
			m_apModel[nCnt]->Uninit();

			delete m_apModel[nCnt];

			m_apModel[nCnt] = nullptr;
		}
	}

	// ���[�V�����̏I������
	m_pMotion->Uninit();

	// ���[�V�����̔j��
	if (m_pMotion != nullptr)
	{
		delete m_pMotion;

		m_pMotion = nullptr;
	}

	Release();
}

//===================================================
// �X�V����
//===================================================
void CPlayer::Update(void)
{
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// �R���g���[���[�̎擾
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// ���b�V���t�B�[���h�̎擾
	CMeshField* pMesh = CManager::GetMeshField();

	// ImGui�̎擾
	CImGuiManager* pImGui = CRenderer::GetImGui();

	static bool bViewMode = false;

	if (m_pMotion->GetLoad() == false)
	{
		return;
	}
	// �v���C���[�̃��f���̐ݒ菈��
	bViewMode = SetPlayerModelElement(pImGui);

	if (bViewMode == true)
	{
		// �ړ�����
		if (pJoypad->GetJoyStickL() == true)
		{
			// �p�b�h�̈ړ�����
			CPlayer::MoveJoypad(pJoypad);
		}
		else
		{
			// �L�[�{�[�h�̈ړ�����
			if (CPlayer::MoveKeyboard(pKeyboard))
			{
				// �_�b�V�����[�V�������������[�V�������𔻒�
				int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

				// �W�����v��jump����Ȃ����𔻒�
				int motiontype = m_bJump ? isDashMotion : MOTIONTYPE_JUMP;

				// ���[�V�����̐ݒ�
				m_pMotion->SetMotion(motiontype, true, 5);

				m_bMotionView = false;
			}
		}

		if (pKeyboard->GetPress(DIK_LSHIFT) || pJoypad->GetPress(pJoypad->JOYKEY_RIGHT_SHOULDER))
		{
			m_bDash = true;
		}
		else
		{
			m_bDash = false;
		}
	}

	// �ړ��ʂ̌���
	m_move.x += (0.0f - m_move.x) * 0.25f;
	m_move.z += (0.0f - m_move.z) * 0.25f;

	// �ʒu�̍X�V
	m_pos += m_move;

	float fHeight = 0.0f;

	// ���b�V���t�B�[���h�̓����蔻��
	if (pMesh->Collision(m_pos,&fHeight))
	{
		m_pos.y = fHeight;

		if (m_bJump == false && m_bMotionView == false)
		{
			m_pMotion->SetMotion(MOTIONTYPE_LANDING, true, 5);
			CMeshCircle::Create(m_pos, 18, 1, 10.0f, 80.0f, 10.0f, 60, D3DXCOLOR(1.0f, 1.0f, 0.5f, 1.0f));
			m_bJump = true;
		}
	}

	// �d�͂����Z
	m_move.y += -MAX_GLABITY;

	if (m_pMotion->GetEventFrame(MOTIONTYPE_MOVE))
	{
		// ���̐���
		CExplosion::Create(D3DXVECTOR3(m_pos.x,m_pos.y + 5.0f,m_pos.z), D3DXVECTOR3(15.0f, 15.0f, 0.0f), WHITE, 4, 3, 5);
	}

	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 				// �n�ʂ̖@���x�N�g���̎擾
		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // ������x�N�g���̍쐬

		// �n�ʂ̊p�x�ɍ��킹���p�x���擾
		D3DXVECTOR3 rot = m_pShadow->GetFieldAngle(FieldNor, PlayerRay);

		// �e�̐ݒ菈��
		m_pShadow->Setting(D3DXVECTOR3(m_pos.x, m_pos.y - fHeight, m_pos.z),D3DXVECTOR3(m_pos.x, fHeight + 2.0f, m_pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT,SHADOW_A_LEVEL);

		m_pShadow->SetRotaition(rot);
	}

	// �W�����v�ł���Ȃ�
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		m_pMotion->SetMotion(MOTIONTYPE_JUMP, true, 5);

		// �ړ��ʂ�������ɐݒ�
		m_move.y = m_fJumpHeight;
		m_bJump = false;
	}

	if (pKeyboard->GetTrigger(DIK_RETURN))
	{
		m_pMotion->SetMotion(MOTIONTYPE_ACTION, true,5);
	}

	// �v���C���[�̃��[�V�����̑J��
	TransitionMotion();

	if (m_pMotion != nullptr && bViewMode == true)
	{
		// ���[�V�����̍X�V����
		m_pMotion->Update(&m_apModel[0], m_nNumModel,m_bMotionView);
	}

	// �����̍���
	float fDiffRot = m_rotDest.y - m_rot.y;

	// �p�x�̐��K��
	NormalizeDiffRot(fDiffRot, &m_rot.y);

	// ������ړI�̌����ɋ߂Â���
	m_rot = LerpDest(m_rotDest, m_rot, 0.1f);
}

//===================================================
// �`�揈��
//===================================================
void CPlayer::Draw(void)
{
	// �����_���[�̎擾
	CRenderer *pRenderer =  CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		// ���f������������
		if (m_apModel[nCnt] != nullptr)
		{
			if (m_bViewMode == true)
			{
				// �`�揈��
				m_apModel[nCnt]->Draw();

				continue;
			}
			if (m_nModelIndex == nCnt)
			{
				// �`�揈��
				m_apModel[nCnt]->Draw(0.5f);
			}
			else
			{
				// �`�揈��
				m_apModel[nCnt]->Draw();
			}
		}
	}
}

//===================================================
// �L�[�{�[�h�̈ړ�����
//===================================================
bool CPlayer::MoveKeyboard(CInputKeyboard* pKeyboard)
{
	bool bMove = false;

	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̌���
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// ����
	float fSpeed = m_bDash ? m_fSpeed : m_fSpeed * 0.15f;

	if (pKeyboard->GetPress(DIK_A))
	{
		//�v���C���[�̈ړ�(��)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			m_move.x += sinf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;
			m_move.z += cosf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;

			m_rotDest.y = cameraRot.y + D3DX_PI * 0.75f;

			bMove = true;
		}
		//�v���C���[�̈ړ�(��)
		else if (pKeyboard->GetPress(DIK_S))
		{
			m_move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			m_move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			m_rotDest.y = cameraRot.y + D3DX_PI * 0.25f;

			bMove = true;
		}
		// �v���C���[�̈ړ�(��)
		else
		{
			m_move.z += sinf(cameraRot.y) * fSpeed;
			m_move.x -= cosf(cameraRot.y) * fSpeed;

			m_rotDest.y = cameraRot.y + D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//�v���C���[�̈ړ�(�E)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//�v���C���[�̈ړ�(��)
		if (pKeyboard->GetPress(DIK_W))
		{
			m_move.x += sinf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;
			m_move.z += cosf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;

			m_rotDest.y = cameraRot.y - D3DX_PI * 0.75f;

			bMove = true;
		}
		//�v���C���[�̈ړ�(��)
		else if (pKeyboard->GetPress(DIK_S))
		{
			m_move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			m_move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			m_rotDest.y = cameraRot.y - D3DX_PI * 0.25f;

			bMove = true;
		}
		// �v���C���[�̈ړ�(�E)
		else
		{
			m_move.z -= sinf(cameraRot.y) * fSpeed;
			m_move.x += cosf(cameraRot.y) * fSpeed;

			m_rotDest.y = cameraRot.y - D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//�v���C���[�̈ړ�(��)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		m_move.x += sinf(cameraRot.y) * fSpeed;
		m_move.z += cosf(cameraRot.y) * fSpeed;

		m_rotDest.y = cameraRot.y + D3DX_PI;

		bMove = true;
	}
	//�v���C���[�̈ړ�(��)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		m_move.x -= sinf(cameraRot.y) * fSpeed;
		m_move.z -= cosf(cameraRot.y) * fSpeed;

		m_rotDest.y = cameraRot.y;

		bMove = true;
	}
	else
	{
		int motiontype = m_pMotion->GetBlendMotionType();

		if ((motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH) && m_bMotionView == false)
		{
			m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);
		}
	}

	return bMove;
}

//===================================================
// �p�b�h�̈ړ�����
//===================================================
void CPlayer::MoveJoypad(CInputJoypad* pJoypad)
{
	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̌���
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// ����
	float fSpeed = m_bDash ? MOVE_SPEED : 2.0f;

	// L�X�e�B�b�N�̊p�x
	float LStickAngleY = pStick->Gamepad.sThumbLY;
	float LStickAngleX = pStick->Gamepad.sThumbLX;

	// �f�b�h�]�[��
	float deadzone = 32767.0f * 0.25f;

	// �X�e�B�b�N�̌X�����p�x�����߂�
	float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

	// ��������
	if (magnitude > deadzone)
	{
		// �A���O���𐳋K��
		float normalizeX = (LStickAngleX / magnitude);
		float normalizeY = (LStickAngleY / magnitude);

		// �v���C���[�̈ړ���
		float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
		float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

		// �ړ��ʂ��X�e�B�b�N�̊p�x�ɂ���ĕύX
		float speedWk = fSpeed * ((magnitude - deadzone) / (32767.0f - deadzone));

		// �v���C���[�̈ړ�
		m_move.x += moveX * speedWk;
		m_move.z += moveZ * speedWk;

		// �v���C���[�̊p�x���ړ������ɂ���
		m_rotDest.y = atan2f(-moveX, -moveZ);

		// �_�b�V�����[�V�������������[�V�������𔻒�
		int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

		// �W�����v��jump����Ȃ����𔻒�
		int motiontype = m_bJump ? isDashMotion : MOTIONTYPE_JUMP;

		m_pMotion->SetMotion(motiontype, true, 5);
	}
	else
	{
		// ���[�V�����̎�ނ��擾
		int motiontype = m_pMotion->GetBlendMotionType();

		// �ړ����[�V������������j���[�g�����ɖ߂�
		if (motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH)
		{
			m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);
		}
	}
	
}

//===================================================
// �v���C���[�̗v�f�̐ݒ菈��
//===================================================
bool CPlayer::SetPlayerModelElement(CImGuiManager *pImGui)
{
	// �ʒu,�傫���̐ݒ�
	pImGui->SetPosition(ImVec2(0.0f, 0.0f));
	pImGui->SetSize(ImVec2(400.0f, 500.0f));

	// �`��J�n
	pImGui->Start("motion", CImGuiManager::TYPE_DEFOULT);

	// ���f�����ǂݍ��܂�Ė���������
	if (m_pMotion->GetLoad() == false)
	{
		ImGui::Text(u8"�ǂݍ��݂Ɏ��s���܂����B[ �G���[ ]");

		pImGui->End();

		return false;
	}

	//ImGui::ShowStyleEditor();

	D3DXVECTOR3 Angle = m_apModel[m_nModelIndex]->GetRotaition();
	D3DXVECTOR3 pos = m_apModel[m_nModelIndex]->GetPosition();;

	if (ImGui::RadioButton(u8"�r���[���[�h", m_bViewMode))
	{
		m_bViewMode = m_bViewMode ? false : true;

		if (m_bViewMode == false)
		{
			// ���f���̈ʒu�̃��Z�b�g
			m_pMotion->Reset(&m_apModel[0],0);
		}
	}

	ImGui::SameLine();

	const char* cMode = m_bViewMode ? "ON" : "OFF";

	ImGui::Text(u8"[ %s ]", cMode);

	// ���[�V�����̏��̐ݒ�
	m_pMotion->MotionSetElement(pImGui, &m_apModel[0], m_bViewMode);

	if (m_bViewMode == true)
	{
		ImGui::PushItemWidth(150); // ��250�s�N�Z���ɂ���

		// �ړI�̒����_
		D3DXVECTOR3 posRDest;

		// �����_���Z�o
		posRDest.x = m_pos.x + sinf(m_rotDest.y) * 1.0f;
		posRDest.y = m_pos.y + sinf(m_rotDest.y) * 1.0f;
		posRDest.z = m_pos.z + cosf(m_rotDest.y) * 1.0f;

		// �J�����̎擾����
		CCamera* pCamera = CManager::GetCamera();

		// �J�����Ǐ]���邩�ǂ���
		static bool bTracking = false;

		if (ImGui::Button(bTracking ? u8"�J�����Ǐ] [ ON ] " : u8"�J�����Ǐ] [ OFF ]"))
		{
			// �t���O�̓���ւ�
			bTracking = bTracking ? false : true;
		}

		// �Ǐ]�o������
		if (bTracking == true)
		{
			// �J�����̒Ǐ]����
			pCamera->SetTracking(posRDest, 1.0f, 0.1f);
		}

		ImGui::DragFloat(u8"�v���C���[�̈ړ���", &m_fSpeed, 0.1f, 0.5f, 100.0f);

		ImGui::DragFloat(u8"�v���C���[�̃W�����v��", &m_fJumpHeight, 0.1f, 0.5f, 100.0f);

		ImGui::PopItemWidth();

	}
	else
	{
		// ����������u���b�N
		if (m_bViewMode == true)
		{
			ImGui::BeginDisabled(); // ����ňȍ~��UI���O���[�A�E�g������s�ɂȂ�
		}

		ImGui::PushItemWidth(100); // ��100�s�N�Z���ɂ���

		// ���f���̃C���f�b�N�X�̕ύX
		if (ImGui::SliderInt(u8"���f���I��", &m_nModelIndex, 0, m_nNumModel - 1))
		{
			// �͈͊O��������N�����v����
			m_nModelIndex = Clamp(m_nModelIndex, 0, m_nNumModel - 1);
		}

		int nParent = m_apModel[m_nModelIndex]->GetParentID();
		
		ImGui::SameLine();

		// ���f���̃C���f�b�N�X�̕ύX
		if (ImGui::SliderInt(u8"�e���f���I��", &nParent, -1, m_nNumModel - 1))
		{
			// �͈͊O��������N�����v����
			nParent = Clamp(nParent, -1, m_nNumModel - 1);

			// �e���f����ID�̐ݒ�
			m_apModel[m_nModelIndex]->SetParentID(nParent);

			// �e���f���̐ݒ�
			if (nParent != -1)
			{// �e������
				m_apModel[m_nModelIndex]->SetParent(m_apModel[nParent]);
			}
			else
			{
				// �e�����Ȃ�
				m_apModel[m_nModelIndex]->SetParent(nullptr);
			}
		}
		ImGui::Spacing();  // ���������c�ɃX�y�[�X������

		ImGui::PopItemWidth();

		ImGui::PushItemWidth(250); // ��250�s�N�Z���ɂ���

		// �X���C�_�[
		if (ImGui::DragFloat3(u8"���f���̈ʒuXYZ", pos, 1.0f))
		{
			// �ʒu�̐ݒ菈��
			m_apModel[m_nModelIndex]->SetPosition(pos);
		}
		//ImGui::NewLine();  // 1�s���̃X�y�[�X�i���s�j
		ImGui::Spacing();  // ���������c�ɃX�y�[�X������
		//ImGui::Dummy(ImVec2(0.0f, 5.0f));  // ����10�s�N�Z���̋�

		// �X���C�_�[
		if (ImGui::DragFloat3(u8"���f���̌���XYZ", Angle,0.05f, -D3DX_PI, D3DX_PI, "%.2f"))
		{
			// �����̐ݒ菈��
			m_apModel[m_nModelIndex]->SetRotaition(Angle);
		}

		// �q���g�̕\��
		if (ImGui::IsItemHovered() && !ImGui::IsItemActive())
		{
			ImGui::SetTooltip(u8"Ctrl�������Ȃ���N���b�N�œ��͉\");
		}

		// �I�t�Z�b�g�̐ݒ�
		if (ImGui::TreeNode(u8"�I�t�Z�b�g�ݒ�"))
		{
			D3DXVECTOR3 offpos = m_apModel[m_nModelIndex]->GetOffPos();
			D3DXVECTOR3 offrot = m_apModel[m_nModelIndex]->GetOffRot();

			// �X���C�_�[
			if (ImGui::DragFloat3(u8"���f���̈ʒuXYZ", offpos, 1.0f))
			{
				// �ʒu�̐ݒ菈��
				m_apModel[m_nModelIndex]->SetOffPos(offpos);
			}
			// �X���C�_�[
			if (ImGui::SliderFloat3(u8"���f���̌���XYZ", offrot, -D3DX_PI, D3DX_PI, "%.2f"))
			{
				// �����̐ݒ菈��
				m_apModel[m_nModelIndex]->SetOffRot(offrot);
			}

			ImGui::TreePop();
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));       // �ʏ���

		if (ImGui::Button(u8"�|�[�Y�̃��Z�b�g"))
		{
			// ���Z�b�g�m�F
			int nID = MessageBox(NULL, "�{���Ƀ��Z�b�g���܂����H", "���Z�b�g", MB_YESNO);

			// �n�C����������
			if (nID == IDYES)
			{
				// ���ׂẴ��f�������Z�b�g����
				for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
				{
					m_apModel[nCnt]->SetPosition(VEC3_NULL);
					m_apModel[nCnt]->SetRotaition(VEC3_NULL);
				}
			}
		}

		ImGui::PopStyleColor(1);  // 3�� Push �����̂�3�� Pop

		ImGui::PopItemWidth();

		if (ImGui::Button(u8"�|�[�Y�̃R�s�["))
		{
			for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
			{
				// ���f���̈ʒu�̎擾
				D3DXVECTOR3 modelpos = m_apModel[nCnt]->GetPosition();
				D3DXVECTOR3 modelrot = m_apModel[nCnt]->GetRotaition();

				// �|�[�Y�̃Z�[�u
				m_apModel[nCnt]->SavePose(modelpos, modelrot);
			}
		}

		ImGui::SameLine(); // ���ɕ��ׂ�

		if (ImGui::Button(u8"�|�[�Y�̃y�[�X�g"))
		{
			for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
			{
				// �|�[�Y�̐ݒ�
				m_apModel[nCnt]->SetPose();
			}
		}

		ImGui::SameLine(); // ���ɕ��ׂ�

		if (ImGui::Button(u8"�|�[�Y�̓o�^"))
		{
			for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
			{
				// �ʒu�̎擾
				D3DXVECTOR3 modelpos = m_apModel[nCnt]->GetPosition();
				D3DXVECTOR3 modelrot = m_apModel[nCnt]->GetRotaition();

				// �L�[�̐ݒ�
				m_pMotion->SetKey(modelpos, modelrot,nCnt);
			}
		}

		char filePath[MAX_PATH] = { "data/key_info.txt" };  // �ۑ�����t�@�C���̃p�X���i�[

		if (ImGui::Button(u8"���[�V�����̃Z�[�u"))
		{
			int fiter = 0;
			// �t�@�C���ۑ��_�C�A���O��\��
			if (ShowSaveFileDialog(filePath, MAX_PATH,&fiter))
			{
				switch (fiter)
				{
				case 1:
					// ���[�V�����̃f�[�^�̏����o��
					m_pMotion->SaveDataTxt(filePath);
					break;
				case 2:
					break;
				default:
					break;
				}
			}
		}

		if (ImGui::Button(u8"�I�t�Z�b�g�̃Z�[�u"))
		{
			int fiter = 0;
			// �t�@�C���ۑ��_�C�A���O��\��
			if (ShowSaveFileDialog(filePath, MAX_PATH, &fiter))
			{
				switch (fiter)
				{
				case 1:
					// ���[�V�����̃f�[�^�̏����o��
					SaveOffset(filePath);
					break;
				case 2:
					break;
				default:
					break;
				}
			}
		}

		//if (ImGui::Button("json"))
		//{
		//	m_pMotion->SaveDataJson(NULL);
		//}


		if (m_bViewMode == true)
		{
			ImGui::EndDisabled(); // ���������I��
		}
	}
	pImGui->End();

	return m_bViewMode;
}

//===================================================
// �v���C���[�̃I�t�Z�b�g�̃Z�[�u
//===================================================
void CPlayer::SaveOffset(const char *pFileName)
{
	// �����o���t�@�C�����J��
	ofstream outFile(pFileName);
	string line;

	// �t�@�C�����J������
	if (outFile.is_open())
	{
		// �����������o��
		outFile << "#--------------------------------------------------------\n";
		outFile << "# �L�����N�^�[���\n";
		outFile << "#--------------------------------------------------------\n";

		outFile << "CHARACTERSET\n";
		outFile << "\tNUM_PARTS = " << m_nNumModel << "\t\t# �p�[�c�̑���\n";

		// �����_�ȉ�����������
		outFile << std::fixed << std::setprecision(2);

		outFile << "\tMOVE = " << m_fSpeed << "\t\t# �ړ���\n";

		outFile << "\tJUMP = " << m_fJumpHeight << "\t\t# �W�����v��\n";

		// ���Ƃɖ߂�
		outFile << std::defaultfloat;

		outFile << "\n";

		// ���f���̏��̕ۑ�
		for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
		{
			outFile << "\tPARTSSET\n";

			outFile << "\t\tINDEX = " << nCnt << "\t\t\t\t\t# \n";

			outFile << "\t\tPARENT = " << m_apModel[nCnt]->GetParentID() << "\t\t\t\t\t# �e���f��()\n";

			outFile << "\t\tPOS = ";
			outFile << std::fixed << std::setprecision(2);

			// �I�t�Z�b�g��ۑ�
			D3DXVECTOR3 offPos = m_apModel[nCnt]->GetOffPos();

			outFile << offPos.x << " ";
			outFile << offPos.y << " ";
			outFile << offPos.z << "\t\t# �ʒu\n";

			outFile << "\t\tROT = ";

			// �I�t�Z�b�g��ۑ�
			D3DXVECTOR3 offRot = m_apModel[nCnt]->GetOffRot();

			outFile << offRot.x << " ";
			outFile << offRot.y << " ";
			outFile << offRot.z << "\t\t# ����\n";

			outFile << std::defaultfloat;

			outFile << "\tEND_PARTSSET\n\n";
		}

		outFile << "END_CHARACTERSET\n";

		// �t�@�C�������
		outFile.close();
	}
}

//===================================================
// system.ini�t�@�C��
//===================================================
void CPlayer::LoadSystemIni(void)
{
	// �t�@�C�����J��
	fstream file("data/system.ini");
	string line;
	string input;

	// �t�@�C�����J������
	if (file.is_open())
	{
		// �Ō�̍s�ɂȂ�܂œǂݍ���
		while (getline(file, line))
		{
			size_t equal_pos = line.find("="); // =�̈ʒu

			// [=] ���������߂�
			input = line.substr(equal_pos + 1);

			// FILENAME��ǂݎ������
			if (line.find("FILENAME") != string::npos)
			{
				// �擪�� = ������
				input.erase(0, input.find_first_not_of(" = "));

				// input���琔�l�����o������
				istringstream value_Input(input);

				// ���f���̖��O�i�[�p�ϐ�
				string FileName;

				// ���l��������
				value_Input >> FileName;

				// ���f���̖��O����
				const char* FILE_NAME = FileName.c_str();

				m_pMotion = CMotion::Load(FILE_NAME, &m_apModel[0], &m_nNumModel, NUM_PARTS);
			}
		}
		file.close();
	}
}

//===================================================
// �v���C���[�̐ݒ菈��
//===================================================
void CPlayer::SetPlayer(const float fSpeed, const float fJumpHeight)
{
	m_fSpeed = fSpeed;
	m_fJumpHeight = fJumpHeight;
}

//===================================================
// �v���C���[�̃��[�V�����̑J��
//===================================================
void CPlayer::TransitionMotion(void)
{
	// ���[�V�����̎��
	MOTIONTYPE motiontype = (MOTIONTYPE)m_pMotion->GetBlendMotionType();

	// ���[�V�����̑J��
	switch (motiontype)
	{
	case MOTIONTYPE_NEUTRAL:
		break;
	case MOTIONTYPE_MOVE:
		break;
	case MOTIONTYPE_ACTION:
		break;
	default:
		break;
	}
}


//===================================================
// ��������
//===================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = nullptr;

	// 3D�I�u�W�F�N�g�̐���
	pPlayer = new CPlayer;

	// �D�揇�ʂ̎擾
	int nPriority = pPlayer->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pPlayer != nullptr)
	{
		// �����̃|�C���^�̉��
		pPlayer->Release();

		// null�ɂ���
		pPlayer = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pPlayer == nullptr) return nullptr;

	pPlayer->m_pos = pos;
	pPlayer->m_rot = rot;

	return pPlayer;
}
