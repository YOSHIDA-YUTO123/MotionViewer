//===================================================
//
// ���[�V���� [motion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _MOTION_H_
#define _MOTION_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"model.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include"imguimaneger.h"

//***************************************************
// �}�N����`
//***************************************************
#define MAX_PARTS (32)  // �p�[�c�̍ő吔
#define MAX_KEY (32)    // �L�[�̍ő吔
#define MAX_MOTION (15) // ���[�V�����̑���
#define MAX_EVENT (10)  // �C�x���g�t���[���̑���

//***************************************************
// �O���錾
//***************************************************
class CModel;
class CMotionLoader;

//***************************************************
// ���[�V�����N���X�̒�`
//***************************************************
class CMotion
{
public:

	// �L�[�̍\����
	struct Key
	{
		float fPosX; // �ʒuX
		float fPosY; // �ʒuY
		float fPosZ; // �ʒuZ

		float fRotX; // ����X
		float fRotY; // ����Y
		float fRotZ; // ����Z
	};

	// �L�[���̍\����
	struct Key_Info
	{
		int nFrame;				// �Đ��t���[��
		Key aKey[MAX_PARTS];	// �e�L�[�p�[�c�̃L�[�v�f
	};

	// �C�x���g�t���[���\����
	struct EventFrame
	{
		int nStart, nEnd; // �J�n�t���[���A�I���t���[��
	};

	// ���[�V�������̍\����
	struct Info
	{
		bool bLoop;								// ���[�v���邩�ǂ���
		int nNumkey;							// �L�[�̑���
		EventFrame aEventFrame[MAX_EVENT];		// �t���[���̐ݒ�
		int nNumEvent;							// ����
		Key_Info aKeyInfo[MAX_KEY];				// �L�[���
	};

	CMotion();
	~CMotion();

	void Init(void);
	static CMotion* Load(const char* pFileName, CModel** ppModel, int* OutNumModel, const int nMaxSize);
	void Uninit(void);
	void Update(CModel** pModel, const int nNumModel,bool bView);
	void SetMotion(const int motiontype, bool bBlend, const int nBlendFrame);
	bool IsEndMotion(void);
	bool IsFinishEndBlend(void);
	void MotionSetElement(CImGuiManager* pImGui, CModel** ppModel, bool bViewMode);
	void Reset(CModel **ppModel, const int nKey);
	void SetInfo(Info* pInfo);
	void InitInfo(void);

	int GetMotionType(void) const { return m_nType; }
	int GetBlendMotionType(void) const { return m_nTypeBlend; }
	void SetKey(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int nIdx);
	void SaveDataTxt(const char* pFileName);
	void SaveDataJson(const char* pFileName);
	bool GetLoad(void) const { return m_pLoader ? true : false; }
private:
	void UpdateCurrentMotion(CModel** pModel, int nIdx);
	void UpdateBlendMotion(CModel** pModel, int nIdx);
	void FinishFirstBlend(void);

	bool m_bLoad;
	CMotionLoader* m_pLoader;		// ���[�_�[
	Info m_aInfo[MAX_MOTION];		// ���[�V�������ւ̃|�C���^
	int m_nType;				// ���[�V�����̎��
	int m_nNumKey;					// �L�[�̑���
	int m_nKey;						// ���݂̃L�[No.
	int m_nCount;				// ���[�V�����̃J�E���^�[
	int m_nextKey;					// ���̃L�[
	bool m_bLoopMotion;				// ���[�v���邩�ǂ���

	bool m_bBlend;			// �u�����h�����邩�ǂ���
	bool m_bLoopMotionBlend;		// ���[�v���邩�ǂ���
	bool m_bFinish;			// ���[�V�������I��������ǂ���
	bool m_bFirst;			// ���[�V�������n�܂����t���O

	int m_nTypeBlend;			// �u�����h�̎��
	int m_nFrameBlend;				// �u�����h�̃t���[����
	int m_nCounterBlend;			// �u�����h�J�E���^�[
	int m_nNumKeyBlend;				// �u�����h���[�V�����̍ő�̃L�[
	int m_nKeyBlend;				// �u�����h���[�V�����̌��݂̃L�[
	int m_nNextKeyBlend;			// �u�����h���[�V�����̎��̃L�[
	int m_nCounterMotionBlend;		// �u�����h�̃J�E���^�[

	int m_nSelectKey;				// ���ݑI�𒆂̃L�[
	int m_nSelectMotion;			// �I�𒆂̃��[�V����
	int m_nIdxEvent;				// �C�x���g�t���[���̃C���f�b�N�X

	int m_nAllFrame;				// �S�̂̃t���[��
	int m_nAllCount;				// �S�̂̃t���[���̃J�E���^�[
};

//***************************************************
// ���[�V�����̃��[�h�N���X�̒�`
//***************************************************
class CMotionLoader
{
public:
	CMotionLoader();
	~CMotionLoader();

	void SetNumModel(const int nNumModel) { m_nNumModel = nNumModel; }
	void SetNumMotion(const int nNumMotion) { m_nNumMotion = nNumMotion; }

	void GetInfo(CMotion::Info* pInfo);
	int GetNumModel(void) const { return m_nNumModel; }
	int GetNumMotion(void) const { return m_nNumMotion; }

protected:
	CMotion::Info m_aInfo[MAX_MOTION]; // ���[�V�����̏��
private:
	int m_nNumModel;			// ���f���̍ő吔
	int m_nNumMotion;			// ���[�V�����̑���
};

//***************************************************
// ���[�V�����̃��[�h�N���X(textFile)�̒�`
//***************************************************
class CLoderText : public CMotionLoader
{
public:
	static CLoderText* LoadTextFile(const char* pFileName, CModel** ppModel, CMotion::Info* pInfo, int* OutNumModel, const int nMaxSize);
private:
	bool LoadModel(CModel** ppModel, const int nMaxSize, int nCnt, std::string input, std::string line);
	bool LoadCharacterSet(CModel** ppModel, std::string line, std::string input);
	void LoadMotionSet(CLoderText* pLoader, std::ifstream& File, std::string nowLine, const int nNumMotion);
	std::istringstream SetInputvalue(std::string input);
};
#endif