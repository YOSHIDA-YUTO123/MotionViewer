//===================================================
//
// �v���C���[ [player.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _PLAYER_H_
#define _PLAYER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// �}�N����`
//***************************************************
#define NUM_PARTS (25) // ���f���̍ő吔

//***************************************************
// �O���錾
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CModel;
class CMotion;
class CShadow;
class CScoreLerper;
class CImGuiManager;

//***************************************************
// �v���C���[�N���X�̒�`
//***************************************************
class CPlayer : public CObject
{
public:

	// ���[�V�����̎��
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0,
		MOTIONTYPE_MOVE,
		MOTIONTYPE_ACTION,
		MOTIONTYPE_JUMP,
		MOTIONTYPE_LANDING,
		MOTIONTYPE_DASH,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CPlayer(int nPriority = 3);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool MoveKeyboard(CInputKeyboard *pKeyboard);
	void MoveJoypad(CInputJoypad* pJoypad);
	bool SetPlayerModelElement(CImGuiManager* pImGui);
	void SaveOffset(const char* pFileName);
	void LoadSystemIni(void);
	void SetPlayer(const float fSpeed, const float fJumpHeight);

	void SetPos(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(const D3DXVECTOR3 rot) { m_rot = rot; }
	void StartMotionView(void) { m_bMotionView = true; }
	D3DXVECTOR3 GetPos(void) const { return m_pos; }
	D3DXVECTOR3 GetRot(void) const { return m_rot; }
private:
	void TransitionMotion(void);

	CMotion *m_pMotion;				// ���[�V�����̃N���X�ւ̃|�C���^
	CModel* m_apModel[NUM_PARTS];	// ���f���N���X�ւ̃|�C���^
	CShadow* m_pShadow;				// �e�N���X�ւ̃|�C���^

	D3DXVECTOR3 m_pos;				// �ʒu
	D3DXVECTOR3 m_rot;				// ����
	D3DXVECTOR3 m_rotDest;			// �ړI�̌���
	D3DXVECTOR3 m_move;				// �ړ���
	D3DXMATRIX m_mtxWorld;			// ���[���h�}�g���b�N�X

	float m_fSpeed;					// �ړ����x
	float m_fJumpHeight;			// �W�����v��

	int m_nModelIndex;				// ���f���̃C���f�b�N�X
	int m_nParentIndex;				// �e���f���̃C���f�b�N�X
	int m_nNumModel;				// ���f���̍ő吔
	int m_nSmockModel;				// �����o�����f��		
	bool m_bViewMode;				// �r���[���[�h���ǂ���
	bool m_bMotionView;				// ���[�V�����̃r���[���ǂ���
	bool m_bJump;					// �W�����v�ł��邩�ǂ���
	bool m_bDash;					// �����Ă邩�ǂ���
};

#endif