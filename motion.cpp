//===================================================
//
// ���[�V���� [motion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "motion.h"
#include<stdio.h>
#include"object.h"
#include"math.h"
#include "manager.h"
#include <filesystem>
#include <iomanip>
#include"json_v3_10_5.hpp"
#include<fstream>

using json = nlohmann::json;
using namespace std;

//***************************************************
// �}�N����`
//***************************************************
#define NEUTRAL (0) // �j���[�g����
#define OFFSET_FRAME (40) // �t���[���̏����

//===================================================
// �R���X�g���N�^
//===================================================
CMotion::CMotion()
{
	ZeroMemory(&m_aInfo, sizeof(m_aInfo));

	m_bLoopMotion = false;
	m_nCount = NULL;
	m_nextKey = NULL;
	m_nKey = NULL;
	m_nNumKey = NULL;
	//m_nNumMotion = NULL;
	//m_nNumModel = NULL;
	m_nType = NULL;
	m_nTypeBlend = NULL;
	m_bFinish = false;
	m_bFirst = false;
	m_bBlend = false;
	m_bLoopMotionBlend = false;
	m_nCounterBlend = NULL;
	m_nCounterMotionBlend = NULL;
	m_nNextKeyBlend = NULL;
	m_nFrameBlend = NULL;
	m_nKeyBlend = NULL;
	m_nNumKeyBlend = NULL;
	m_nSelectKey = NULL;
	m_nSelectMotion = NULL;
	m_nAllCount = NULL;
	m_nAllFrame = NULL;
	m_bLoad = false;
	m_nIdxEvent = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CMotion::~CMotion()
{
}

//===================================================
// ����������
//===================================================
void CMotion::Init(void)
{
	if (m_pLoader == nullptr)
	{
		return;
	}
	// ���[�V�����̑���
	int nNumMotion = m_pLoader->GetNumMotion();

	// ���[�V�����̑�����
	for (int nCntMotion = 0; nCntMotion < nNumMotion; nCntMotion++)
	{
		// �L�[�̑���
		int nNumKey = m_aInfo[nCntMotion].nNumkey;

		// �L�[�̑�����
		for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
		{
			// �t���[����0�ȉ���������
			if (m_aInfo[nCntMotion].aKeyInfo[nCntKey].nFrame <= 0)
			{
				// ��l�����Ă���
				m_aInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = OFFSET_FRAME;
			}
		}
	}
}

//===================================================
// ���[�V�����̃��[�h����
//===================================================
CMotion* CMotion::Load(const char* pFileName, CModel** ppModel, int* OutNumModel, const int nMaxSize)
{
	CMotion::Info Info;

	CMotion* pMotion = new CMotion;

	if (pMotion != nullptr)
	{
		pMotion->m_pLoader = CLoderText::LoadTextFile(pFileName, ppModel, &Info, OutNumModel, nMaxSize);

		// ���[�h���������Ă�����
		if (pMotion->m_pLoader != nullptr)
		{
			pMotion->m_pLoader->GetInfo(&pMotion->m_aInfo[0]);
		}
	}

	return pMotion;
}

//===================================================
// �L�[���̐ݒ菈��
//===================================================
void CMotion::SetKey(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot,const int nIdx)
{
	// �L�[�̐ݒ�
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fPosX = pos.x;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fPosY = pos.y;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fPosZ = pos.z;

	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fRotX = rot.x;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fRotY = rot.y;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fRotZ = rot.z;
}

//===================================================
// �Z�[�u����(�e�L�X�g�t�@�C��)
//===================================================
void CMotion::SaveDataTxt(const char *pFileName)
{
	// �����o���t�@�C�����J��
	ofstream outFile(pFileName);
	string line;

	// �t�@�C�����J������
	if (outFile.is_open())
	{
		// �����������o��
		outFile << "+--------------------------------------------------------+" << endl;
		outFile << "+	�L�[��� [data/key_info.txt]						 +" << endl;
		outFile << "+--------------------------------------------------------+" << endl;

		outFile << "+--------------------------------------------------------+" << endl;
		outFile << " ���[�V�����^�C�v[]" << endl;
		outFile << "+--------------------------------------------------------+" << endl;

		outFile << "MOTIONSET\n";
		outFile << "	LOOP = " << m_aInfo[m_nSelectMotion].bLoop << "			# ���[�v���邩�ǂ���[0:���[�v���Ȃ� / 1:���[�v����]\n";
		outFile << "	NUM_KEY = " << m_aInfo[m_nSelectMotion].nNumkey << "			# �L�[�̑���\n\n";

		// �C�x���g�t���[���̑���
		int nNumEvent = m_aInfo[m_nSelectMotion].nNumEvent;

		// �C�x���g�t���[��������Ȃ�
		if (nNumEvent > 0)
		{
			outFile << " \tNUM_EVENT = " << nNumEvent << "\t\t\t\t# �C�x���g�t���[���̑���\n";

			outFile << " \tSTART_FRAME = ";

			// �C�x���g�t���[���̏����o��
			for (int nCnt = 0; nCnt < nNumEvent; nCnt++)
			{
				outFile << m_aInfo[m_nSelectMotion].aEventFrame[nCnt].nStart << " ";
			}

			outFile << " \t\t# �J�n�t���[�� << ������0,1,2... >>\n";

			outFile << "\tEND_FRAME = ";

			// �C�x���g�t���[���̏����o��
			for (int nCnt = 0; nCnt < nNumEvent; nCnt++)
			{
				outFile << m_aInfo[m_nSelectMotion].aEventFrame[nCnt].nEnd << " ";
			}

			outFile << "\t\t# �I���t���[��\n\n";
		}
		// �L�[�̑���
		int nNumkey = m_aInfo[m_nSelectMotion].nNumkey;

		// �L�[�̑�������
		for (int nCnt = 0; nCnt < nNumkey; nCnt++)
		{
			// �t���[����
			int nFrame = m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].nFrame;

			// �����������o��
			outFile << "\tKEYSET\t\t\t\t# --- << KEY : " << nCnt << " / " << nNumkey << " >> --- \n";
			outFile << "\t\tFRAME = " << nFrame << endl;

			// ���f���̑���
			int nNumModel = m_pLoader->GetNumModel();

			// ���f���̐�����
			for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
			{
				// �����̏����o��
				outFile << "\t\tKEY # ----- [ " << nCntModel << " ] -----\n";
				outFile << "\t\t\tPOS = ";
				outFile << std::fixed << std::setprecision(2);
				outFile << m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].aKey[nCntModel].fPosX << " ";
				outFile << m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].aKey[nCntModel].fPosY << " ";
				outFile << m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].aKey[nCntModel].fPosZ << endl;

				outFile << "\t\t\tROT = ";

				outFile << m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].aKey[nCntModel].fRotX << " ";
				outFile << m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].aKey[nCntModel].fRotY << " ";
				outFile << m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].aKey[nCntModel].fRotZ << endl;

				outFile << std::defaultfloat;

				outFile << "\t\tEND_KEY\n";

				// �Ō�ȊO�͉��s
				if (nCntModel < nNumModel - 1)
				{
					outFile << "\n";
				}
			}

			outFile << "\tEND_KEYSET\n";

			// �Ō�ȊO�͉��s
			if (nCnt < nNumkey - 1)
			{
				outFile << "\n";
			}
		}

		outFile << "END_MOTIONSET\n";

		// �t�@�C�������
		outFile.close();
	}
	else
	{
		MessageBox(NULL, pFileName, "�t�@�C�����J���܂���ł���", MB_OK);
		return;
	}
}

//===================================================
// ���[�V�����̃Z�[�u����
//===================================================
void CMotion::SaveDataJson(const char* pFileName)
{
	json config;

	std::ofstream file("data/config.json");

	std::map<std::string, json> keyset;

	// json�t�@�C���̏�������
	keyset["MOTIONTYPE"].push_back(m_nSelectMotion);
	keyset["LOOP"].push_back(m_aInfo[m_nSelectMotion].bLoop);

	// �L�[�̑�����ۑ�
	int nNumkey = m_aInfo[m_nSelectMotion].nNumkey;

	keyset["NUM_KEY"].push_back(nNumkey);

	int type = m_nSelectMotion;

	for (int nCnt = 0; nCnt < nNumkey; nCnt++)
	{
		keyset["FRAME"].push_back(m_aInfo[type].aKeyInfo[nCnt].nFrame);

		keyset["KEY"].push_back(nCnt);

		int nNumModel = m_pLoader->GetNumModel();

		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{
			D3DXVECTOR3 motionset;
			motionset.x = m_aInfo[type].aKeyInfo[nCnt].aKey[nCntModel].fPosX;
			motionset.y = m_aInfo[type].aKeyInfo[nCnt].aKey[nCntModel].fPosY;
			motionset.z = m_aInfo[type].aKeyInfo[nCnt].aKey[nCntModel].fPosZ;

			std::map<std::string, json> jpos;

			jpos["INDEX"].push_back(nCntModel);
		}
	}

	config["MOITONSET"] = keyset;

	if (file.is_open())
	{
		file << config.dump(4); // �C���f���g�t���ŕۑ�

		file.close();
	}
	//
	//std::fstream File("data/config.json");

	//bool bLoop = config["Loop"];
	//int nNumKey = config["NumKey"];

	//File.close();

}

//===================================================
// �C�x���g�t���[���̔���
//===================================================
bool CMotion::GetEventFrame(const int motiontype)
{
	// ��ނ��������
	if (m_nType != motiontype) return false;

	// �C�x���g�̑�������
	for (int nCnt = 0; nCnt < m_aInfo[motiontype].nNumEvent; nCnt++)
	{
		// ���肪�͂��܂�����
		if (m_nAllCount >= m_aInfo[motiontype].aEventFrame[nCnt].nStart && m_nAllCount <= m_aInfo[motiontype].aEventFrame[nCnt].nEnd)
		{
			return true;
		}
	}
	return false;
}

//===================================================
// �I������
//===================================================
void CMotion::Uninit(void)
{
	if (m_pLoader != nullptr)
	{
		delete m_pLoader;
		m_pLoader = nullptr;
	}
}

//===================================================
// �ŏ��̃��[�V�����̃u�����h���I�������
//===================================================
void CMotion::FinishFirstBlend(void)
{
	// ���[�V�����̏o�����̃u�����h���I������
	if (m_bFirst == true && m_nCounterBlend >= m_nFrameBlend)
	{
		// ���Ƃɖ߂�
		m_bFirst = false;

		m_nKey = 0;

		// ���[�V�������u�����h�������[�V�����ɂ���
		m_nType = m_nTypeBlend;

		for (int nCnt = 0; nCnt < m_aInfo[m_nType].nNumkey; nCnt++)
		{
			if (m_nFrameBlend >= m_aInfo[m_nType].aKeyInfo[nCnt].nFrame)
			{
				m_nFrameBlend -= m_aInfo[m_nType].aKeyInfo[nCnt].nFrame;
			}
		}
		m_nCount = m_nFrameBlend;
		m_nAllCount = m_nCount;

		m_nCounterBlend = 0;
	}
}

//===================================================
// ���݂̃��[�V�����̍X�V����
//===================================================
void CMotion::UpdateCurrentMotion(CModel** pModel, int nIdx)
{
	// ���̃L�[�̃A�h���X�̎擾
	Key* pKeyNext = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// ���݂̃L�[�̃A�h���X�̎擾
	Key* pKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// �L�[���̃A�h���X�̎擾
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	// ���̃L�[�܂ł̋��������߂�
	float fDiffPosX = pKeyNext->fPosX - pKey->fPosX;
	float fDiffPosY = pKeyNext->fPosY - pKey->fPosY;
	float fDiffPosZ = pKeyNext->fPosZ - pKey->fPosZ;

	float fDiffRotX = pKeyNext->fRotX - pKey->fRotX;
	float fDiffRotY = pKeyNext->fRotY - pKey->fRotY;
	float fDiffRotZ = pKeyNext->fRotZ - pKey->fRotZ;

	// �p�x�̐��K��
	NormalizeRot(&fDiffRotX);
	NormalizeRot(&fDiffRotY);
	NormalizeRot(&fDiffRotZ);

	// �t���[���ƍő�t���[���̊��������߂�
	float fRateFrame = (float)m_nCount / (float)pKeyInfo->nFrame;

	// ���̃L�[�̈ʒu�܂Ńt���[���ɉ������ʒu��ݒ肷��
	float fPosX = LerpDiff(pKey->fPosX, fDiffPosX, fRateFrame);
	float fPosY = LerpDiff(pKey->fPosY, fDiffPosY, fRateFrame);
	float fPosZ = LerpDiff(pKey->fPosZ, fDiffPosZ, fRateFrame);

	// ���̃L�[�̌����܂Ńt���[���ɉ�����������ݒ肷��
	float fRotX = LerpDiff(pKey->fRotX, fDiffRotX, fRateFrame);
	float fRotY = LerpDiff(pKey->fRotY, fDiffRotY, fRateFrame);
	float fRotZ = LerpDiff(pKey->fRotZ, fDiffRotZ, fRateFrame);

	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// �u�����h�̃��[�V�����̍X�V����
//===================================================
void CMotion::UpdateBlendMotion(CModel** pModel, int nIdx)
{
	// ���݂̃L�[�̃A�h���X���擾
	Key* pCurrentKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// ���̃L�[�̃A�h���X���擾
	Key* pNextKey = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// �L�[���̃A�h���X�̎擾
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	float fRate = (float)m_nCount / (float)pKeyInfo->nFrame; // ���Βl

	float fRateBlendMotion = (float)m_nCounterBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame; // ���Βl

	float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;

	// ���݂̃��[�V�����̊p�x�̍���
	float fDiffRX = pNextKey->fRotX - pCurrentKey->fRotX;
	float fDiffRY = pNextKey->fRotY - pCurrentKey->fRotY;
	float fDiffRZ = pNextKey->fRotZ - pCurrentKey->fRotZ;

	//// �p�x�̐��K��
	//NormalizeRot(&fDiffRX);
	//NormalizeRot(&fDiffRY);
	//NormalizeRot(&fDiffRZ);

	// ���݂̃��[�V�����̃t���[���ɉ������p�x��ݒ�
	float fRotXCurrent = pCurrentKey->fRotX + fDiffRX * fRate;
	float fRotYCurrent = pCurrentKey->fRotY + fDiffRY * fRate;
	float fRotZCurrent = pCurrentKey->fRotZ + fDiffRZ * fRate;

	// �u�����h�̃L�[�̃A�h���X���擾
	Key* pCurrentBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nIdx];

	// �u�����h�̎��̃L�[�̃A�h���X���擾
	Key* pNextBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nNextKeyBlend].aKey[nIdx];

	//�u�����h���[�V�����̊p�x�̍��������߂�
	float fDiffBlendRX = pNextBlendKey->fRotX - pCurrentBlendKey->fRotX;
	float fDiffBlendRY = pNextBlendKey->fRotY - pCurrentBlendKey->fRotY;
	float fDiffBlendRZ = pNextBlendKey->fRotZ - pCurrentBlendKey->fRotZ;

	//// �p�x�̐��K��
	//NormalizeRot(&fDiffBlendRX);
	//NormalizeRot(&fDiffBlendRY);
	//NormalizeRot(&fDiffBlendRZ);

	// �u�����h���[�V�����̃t���[���ɉ������p�x�̐ݒ�
	float fRotXBlend = pCurrentBlendKey->fRotX + fDiffBlendRX * fRateBlendMotion;
	float fRotYBlend = pCurrentBlendKey->fRotY + fDiffBlendRY * fRateBlendMotion;
	float fRotZBlend = pCurrentBlendKey->fRotZ + fDiffBlendRZ * fRateBlendMotion;

	//// �p�x�̐��K��
	//NormalizeRot(&fRotXBlend);
	//NormalizeRot(&fRotYBlend);
	//NormalizeRot(&fRotZBlend);

	//���݂̃��[�V�����̈ʒu�̍��������߂�
	float fDiffPX = pNextKey->fPosX - pCurrentKey->fPosX;
	float fDiffPY = pNextKey->fPosY - pCurrentKey->fPosY;
	float fDiffPZ = pNextKey->fPosZ - pCurrentKey->fPosZ;

	// ���݂̃��[�V�����̃t���[���ɉ������ʒu�̐ݒ�
	float fPosXCurrent = pCurrentKey->fPosX + fDiffPX * fRate;
	float fPosYCurrent = pCurrentKey->fPosY + fDiffPY * fRate;
	float fPosZCurrent = pCurrentKey->fPosZ + fDiffPZ * fRate;

	// �u�����h���[�V�����̈ʒu�̍��������߂�
	float fDiffBlendPX = pNextBlendKey->fPosX - pCurrentBlendKey->fPosX;
	float fDiffBlendPY = pNextBlendKey->fPosY - pCurrentBlendKey->fPosY;
	float fDiffBlendPZ = pNextBlendKey->fPosZ - pCurrentBlendKey->fPosZ;

	// �u�����h���[�V�����̃t���[���ɉ������ʒu�̐ݒ�
	float fPosXBlend = pCurrentBlendKey->fPosX + fDiffBlendPX * fRateBlendMotion;
	float fPosYBlend = pCurrentBlendKey->fPosY + fDiffBlendPY * fRateBlendMotion;
	float fPosZBlend = pCurrentBlendKey->fPosZ + fDiffBlendPZ * fRateBlendMotion;

	// ���݂̊p�x�ƃu�����h����p�x�̍��������߂�
	fDiffBlendRX = fRotXBlend - fRotXCurrent;
	fDiffBlendRY = fRotYBlend - fRotYCurrent;
	fDiffBlendRZ = fRotZBlend - fRotZCurrent;

	// �p�x�̐��K��
	NormalizeRot(&fDiffBlendRX);
	NormalizeRot(&fDiffBlendRY);
	NormalizeRot(&fDiffBlendRZ);

	// �u�����h�̃t���[���ɉ����������̐ݒ�
	float fRotX = fRotXCurrent + fDiffBlendRX * fRateBlend;
	float fRotY = fRotYCurrent + fDiffBlendRY * fRateBlend;
	float fRotZ = fRotZCurrent + fDiffBlendRZ * fRateBlend;

	// ���݂̈ʒu�ƃu�����h����ʒu�̍��������߂�
	fDiffBlendPX = fPosXBlend - fPosXCurrent;
	fDiffBlendPY = fPosYBlend - fPosYCurrent;
	fDiffBlendPZ = fPosZBlend - fPosZCurrent;

	// �u�����h�̃t���[���ɉ������ʒu�̐ݒ�
	float fPosX = fPosXCurrent + fDiffBlendPX * fRateBlend;
	float fPosY = fPosYCurrent + fDiffBlendPY * fRateBlend;
	float fPosZ = fPosZCurrent + fDiffBlendPZ * fRateBlend;

	// �ʒu��ݒ�
	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	// ������ݒ�
	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// �X�V����
//===================================================
void CMotion::Update(CModel** pModel,const int nNumModel,bool bView)
{
	for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
	{
		// �L�[�̑���
		m_nNumKey = m_aInfo[m_nType].nNumkey;
		m_nNumKeyBlend = m_aInfo[m_nTypeBlend].nNumkey;

		// ���[�v���邩�ǂ���
		m_bLoopMotion = m_aInfo[m_nType].bLoop;

		//if (motiontype < 0 || motiontype >= TYPE::TYPE_MAX)
		//{
		//	return;
		//}
		
		// �L�[�̑�����0����Ȃ��Ȃ�
		if (m_nNumKey != 0)
		{
			// �̃L�[�𑝂₷
			m_nextKey = (m_nKey + 1) % m_nNumKey;
		}

		// �L�[�̑�����0����Ȃ��Ȃ�
		if (m_nNumKeyBlend != 0)
		{
			// ���̃u�����h�L�[�𑝂₷
			m_nNextKeyBlend = (m_nKeyBlend + 1) % m_nNumKeyBlend;
		}

		if (m_bFinish == false && m_bFirst == false)
		{
			// ���݂̃��[�V�����̍X�V����
			UpdateCurrentMotion(pModel, nCntModel);
		}
		if ((m_bFinish == true || m_bFirst == true) && m_bBlend == true)
		{
			// �u�����h�̃��[�V�����̍X�V����
			UpdateBlendMotion(pModel, nCntModel);
		}
	}	
	
	if (bView == true && IsEndMotion())
	{
		m_bBlend = true;
	}

	// �v�Z�p�ő�t���[��
	int nAllFrame = 0;

	for (int nCnt = 0; nCnt < m_nNumKey; nCnt++)
	{
		// �t���[�������Z
		nAllFrame += m_aInfo[m_nType].aKeyInfo[nCnt].nFrame;
	}

	// �ő�t���[�����Z�b�g
	m_nAllFrame = nAllFrame;

	// �ő�𒴂�����
	if (m_nAllCount >= m_nAllFrame)
	{
		m_nAllCount = 0;
	}

	// ���[�V�������I��������
	if (IsEndMotion())
	{
		m_nKeyBlend = 0;
		m_nCounterBlend = 0;
		m_nFrameBlend = m_aInfo[m_nType].aKeyInfo[m_nNumKey - 1].nFrame;
		m_bFinish = true;
		m_nTypeBlend = NEUTRAL;
	}

	// ���[�V�����̏o�����̃u�����h���I������
	FinishFirstBlend();

	// �L�[���ő傩�u�����h�̃J�E���g���ő�ɂȂ���
	if (IsFinishEndBlend() == true)
	{
		m_nAllCount = m_nFrameBlend;
		m_bFinish = false;			// ���Ƃɖ߂�
		m_bBlend = false;				// ���Ƃɖ߂�
		m_nType = NEUTRAL;				// ���[�V�����^�C�v���j���[�g�����ɂ���
		m_nCount = m_nFrameBlend;
		m_nCounterBlend = 0;
	}

	// �J�E���g���t���[���𒴂�����
	if (m_nCount >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		// �L�[�𑝂₷
		m_nKey++;

		// �͈͓��Ƀ��b�v����
		m_nKey = Wrap(m_nKey, 0, m_aInfo[m_nType].nNumkey - 1);

		// �J�E���g�����Z�b�g
		m_nCount = 0;
	}

	// �u�����h�J�E���^�[���t���[���𒴂�����
	if (m_nCounterBlend >= m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame && (m_bFinish == true || m_bFirst == true))
	{
		// �L�[�𑝂₷
		m_nKeyBlend++;

		// �͈͓��Ƀ��b�v����
		m_nKeyBlend = Wrap(m_nKeyBlend, 0, m_aInfo[m_nTypeBlend].nNumkey - 1);
	}

	// �ŏ��̃u�����h�����Ă��Ȃ�������
	if (m_bFirst == false)
	{
		m_nAllCount++;

		m_nCount++;
	}

	// �u�����h���n�܂�����
	if (m_bFinish == true || m_bFirst == true)
	{
		m_nCounterBlend++;
	}
}

//===================================================
// ���[�V�����̐ݒ菈��
//===================================================
void CMotion::SetMotion(const int motiontype, bool bBlend, const int nBlendFrame)
{
	// ������ނ̃��[�V������������
	if (m_nTypeBlend == motiontype || m_nType == motiontype) return;

	// ���[�V�����̑����𒴂��Ă�����
	if (m_pLoader->GetNumMotion() <= motiontype)
	{
		return;
	}

	// �u�����h������Ȃ�
	if (bBlend == true)
	{
		m_nKeyBlend = 0;

		m_nCounterBlend = 0;		 // �u�����h�J�E���^�[�����Z�b�g
		m_nFrameBlend = nBlendFrame; // �u�����h�t���[����ݒ肷��
		m_bFirst = true;			 // �ŏ��̃u�����h�J�n�t���O��true�ɂ���
		
		//m_nKey = 0;
		m_bFinish = false;			 // ���[�V�������I����Ă��Ȃ�
		m_nTypeBlend = motiontype;   // �u�����h��̎�ނ�ݒ�
	}
	else
	{
		m_nKey = 0;						// �ŏ�����n�߂�
		m_nCount = 0;					// �ŏ�����n�߂�
		m_nType = motiontype;			// �u�����h���郂�[�V�����̃^�C�v����
		m_nTypeBlend = motiontype;		// �u�����h���郂�[�V�����̃^�C�v����
		m_bFinish = false;
	}

	m_bBlend = bBlend; // �u�����h���邩�ǂ�������

	m_nAllCount = 0; // �S�̂̃t���[���̃J�E���^�[�����Z�b�g����
}

//===================================================
// ���[�V�������I��������ǂ���
//===================================================
bool CMotion::IsEndMotion(void)
{
	// ���[�V�������I��������
	if (m_bFinish == false &&
		m_nKey >= m_nNumKey - 1 &&
		m_bLoopMotion == false &&
		m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// �I���̃u�����h����������������
//===================================================
bool CMotion::IsFinishEndBlend(void)
{
	// �u�����h���I��������(�I��肩��)
	if (m_bFinish == true && m_nFrameBlend <= m_nCounterBlend && m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// ���[�V�����̃f�[�^�̐ݒ�
//===================================================
void CMotion::MotionSetElement(CImGuiManager* pImGui, CModel** ppModel, bool bViewMode)
{
	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	// �L�[�������𒴂��Ă�����
	if (m_nSelectKey >= m_aInfo[m_nSelectMotion].nNumkey)
	{
		// �����̂ЂƂO�ɖ߂�
		m_nSelectKey = m_aInfo[m_nSelectMotion].nNumkey - 1;
	}

	ImGui::PushItemWidth(100); // ��100�s�N�Z���ɂ���

		// �{�^���̐�����
	const char* RadioButton = m_aInfo[m_nSelectMotion].bLoop ? u8"���[�v����" : u8"���[�v���Ȃ�";

	if (ImGui::RadioButton(RadioButton, m_aInfo[m_nSelectMotion].bLoop))
	{
		// �t���O�����ւ���
		m_aInfo[m_nSelectMotion].bLoop = m_aInfo[m_nSelectMotion].bLoop ? false : true;
	}

	if (ImGui::SliderInt(u8"���[�V�����I��", &m_nSelectMotion, 0, m_pLoader->GetNumMotion() - 1))
	{
		// �|�[�Y�̃��Z�b�g
		Reset(ppModel, 0);
		m_nSelectKey = 0;
	}

	// �r���[���[�h����Ȃ��Ȃ�
	if (bViewMode == false)
	{
		// ���[�V�����̑���
		int nNum = m_pLoader->GetNumMotion();

		if (ImGui::SliderInt(u8"���[�V�����̑���", &nNum, 0, MAX_MOTION))
		{
			// ���[�V�����̑����̐ݒ�
			m_pLoader->SetNumMotion(nNum);
		}
	}

	ImGui::DragInt(u8"�t���[��", &m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].nFrame, 0.5f, 1, 256);

	// �v�Z�p�ő�t���[��
	int nAllFrame = 0;

	for (int nCnt = 0; nCnt < m_aInfo[m_nSelectMotion].nNumkey; nCnt++)
	{
		// �t���[�������Z
		nAllFrame += m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].nFrame;
	}

	// �r���[���[�h�Ȃ�
	if (bViewMode == true && ImGui::TreeNode(u8"�t���[���̐ݒ�"))
	{
		ImGui::Text(u8"�C�x���g�t���[���̑��� %d / %d", m_aInfo[m_nSelectMotion].nNumEvent,MAX_EVENT);

		// ���ɕ��ׂ�
		ImGui::SameLine();

		ImGui::PushItemWidth(150); // ��150�s�N�Z���ɂ���

		if (ImGui::Button(u8"�{") && m_aInfo[m_nSelectMotion].nNumEvent < MAX_EVENT)
		{
			// �C�x���g�t���[���̑����𑝂₷
			m_aInfo[m_nSelectMotion].nNumEvent++;
		}
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(350); // ��350�s�N�Z���ɂ���

		// ���ɕ��ׂ�
		ImGui::SameLine();

		if (ImGui::Button(u8"�|") && m_aInfo[m_nSelectMotion].nNumEvent >= 1)
		{
			// �C�x���g�t���[���̑��������炷
			m_aInfo[m_nSelectMotion].nNumEvent--;
		}
		// �{�^���̑傫���̐ݒ�
		ImGui::PopItemWidth();

		// �C�x���g�t���[��������Ȃ�
		if (m_aInfo[m_nSelectMotion].nNumEvent > 0)
		{
			ImGui::SliderInt(u8"�C�x���g�t���[���̑I��", &m_nIdxEvent, 0, m_aInfo[m_nSelectMotion].nNumEvent - 1);
		}

		// �C�x���g�t���[���̑�����0����Ȃ�
		if (m_aInfo[m_nSelectMotion].nNumEvent > 0)
		{
			// ���̏�����
			InitInfo();

			// �C�x���g�t���[���̃C���f�b�N�X
			int nIdx = m_nIdxEvent;

			// ���[�V�����̏��̃A�h���X�̎擾
			EventFrame* pEvent = &m_aInfo[m_nSelectMotion].aEventFrame[nIdx];

			// �C�x���g�t���[���̐ݒ�
			ImGui::SliderInt(u8"�J�n�t���[���̐ݒ�", &pEvent->nStart, 1, nAllFrame);
			ImGui::SliderInt(u8"�I���t���[���̐ݒ�", &pEvent->nEnd, pEvent->nStart, nAllFrame);

			// ���[�V�������̕\��
			ImGui::Text(u8"KEY     [ %d / %d ]", m_nKey, m_aInfo[m_nType].nNumkey);
			ImGui::Text(u8"FRAME     [ %d / %d ]", m_nCount, m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame);
			ImGui::Text(u8"ALLFRAME     [ %d / %d ]", m_nAllCount, m_nAllFrame);
		}

		ImGui::TreePop();
	}

	// �r���[���[�h����Ȃ��Ȃ�
	if (bViewMode == false)
	{
		// �L�[�̑����̕ύX����
		ImGui::DragInt(u8"##�L�[�̑����̕ύX", &m_aInfo[m_nSelectMotion].nNumkey, 1.0f, 1, MAX_KEY - 1);

		ImGui::SameLine();

		if (ImGui::Button("+", ImVec2(25.0f, 25.0f)) && m_aInfo[m_nSelectMotion].nNumkey < MAX_KEY - 1)
		{
			m_aInfo[m_nSelectMotion].nNumkey++;
		}
		ImGui::SameLine();

		if (ImGui::Button("-", ImVec2(25.0f, 25.0f)) && m_aInfo[m_nSelectMotion].nNumkey > 1)
		{
			m_aInfo[m_nSelectMotion].nNumkey--;
		}

		ImGui::SameLine();

		ImGui::Text(u8"�L�[�̑����̕ύX");
	}

	// �L�[�̕ύX����
	if (ImGui::SliderInt(u8"�L�[�̕ύX", &m_nSelectKey, 0, m_aInfo[m_nSelectMotion].nNumkey - 1))
	{
		// �|�[�Y�̃��Z�b�g
		Reset(ppModel, m_nSelectKey);

		// �t���[���̏����ݒ�
		InitInfo();

		m_nKey = m_nSelectKey;
	}
	
	if (bViewMode == true)
	{
		if (ImGui::Button(u8"���[�V�����̍Đ�"))
		{
			// �r���[�̊J�n
			pPlayer->StartMotionView();
			SetMotion(m_nSelectMotion, true, 5);
		}
	}

	// �{�^���̑傫���̐ݒ�
	ImGui::PopItemWidth();
}

//===================================================
// ���Z�b�g
//===================================================
void CMotion::Reset(CModel** ppModel,const int nKey)
{
	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	// ���f���̑����̎擾
	int nNumModel = m_pLoader->GetNumModel();

	for (int nCnt = 0; nCnt < nNumModel; nCnt++)
	{
		// ���f���̃L�[���̃A�h���X�̎擾
		Key* pKey = &m_aInfo[m_nSelectMotion].aKeyInfo[nKey].aKey[nCnt];

		// �ʒu�̐ݒ�
		ppModel[nCnt]->SetPosition(D3DXVECTOR3(pKey->fPosX, pKey->fPosY, pKey->fPosZ));
		ppModel[nCnt]->SetRotaition(D3DXVECTOR3(pKey->fRotX, pKey->fRotY, pKey->fRotZ));
	}
	
	// ���̃��Z�b�g
	m_nCount = 0;
	m_nKey = nKey;
	m_nSelectKey = nKey;
	
	if (pPlayer != nullptr)
	{
		// ���[�V�����̐ݒ�
		SetMotion(m_nSelectMotion, false, 0);

		// ���[�V�����̃r���[�̊J�n
		pPlayer->StartMotionView();
	}
}

//===================================================
// ���̐ݒ�
//===================================================
void CMotion::SetInfo(Info* pInfo)
{
	memcpy(m_aInfo, pInfo, sizeof(m_aInfo));
}

//===================================================
// �t���[���̏����ݒ�
//===================================================
void CMotion::InitInfo(void)
{
	// �t���[����0�ɂȂ��Ă�����
	if (m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].nFrame <= 0)
	{
		// �����l�����Ă���
		m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].nFrame = OFFSET_FRAME;
	}

	// �L�[�̑�����0�ȉ���������
	if (m_aInfo[m_nSelectMotion].nNumkey <= 0)
	{
		// 1�ɂ���
		m_aInfo[m_nSelectMotion].nNumkey = 1;
	}

	// �C�x���g�t���[���̃C���f�b�N�X
	int nIdx = m_nIdxEvent;

	// ���[�V�����̏��̃A�h���X�̎擾
	EventFrame* pEvent = &m_aInfo[m_nSelectMotion].aEventFrame[nIdx];

	// �J�n�t���[����0�ȉ���������
	if (pEvent->nStart <= 0)
	{
		// �J�n�t���[����1�ɂ���
		pEvent->nStart = 1;
	}

	// �I���t���[����0�ȉ���������
	if (pEvent->nEnd <= 0)
	{
		// �J�n�t���[����1�ɂ���
		pEvent->nEnd = 1;
	}
}

//===================================================
// �R���X�g���N�^
//===================================================
CMotionLoader::CMotionLoader()
{
	memset(m_aInfo, NULL, sizeof(m_aInfo));
	m_nNumModel = NULL;
	m_nNumMotion = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CMotionLoader::~CMotionLoader()
{
}

//===================================================
// ���̎擾
//===================================================
void CMotionLoader::GetInfo(CMotion::Info* pInfo)
{
	for (int nCntMotion = 0; nCntMotion < m_nNumMotion; nCntMotion++)
	{
		memcpy(&pInfo[nCntMotion], &m_aInfo[nCntMotion], sizeof(pInfo[nCntMotion]));
	}
}

//===================================================
// �e�L�X�g�t�@�C���̃��[�h����
//===================================================
CLoderText* CLoderText::LoadTextFile(const char* pFileName, CModel** ppModel, CMotion::Info* pInfo, int* OutNumModel,const int nMaxSize)
{
	// �t�@�C�������[�h����
	ifstream File(pFileName);
	string line;
	string input;

	int nNumModel = 0;

	D3DXVECTOR3 offset = VEC3_NULL;

	bool bCharacterSet = false;	// �L�����N�^�[�̐ݒ���������ǂ���

	if (File.is_open() == true)
	{
		// ���[�V�����𐶐�
		CLoderText* pLoader = new CLoderText;

		// �Ō�̍s�ɂȂ�܂œǂݍ���
		while (getline(File, line))
		{
			size_t equal_pos = line.find("="); // =�̈ʒu

			// [=] ���������߂�
			input = line.substr(equal_pos + 1);

			// ���f���̃��[�h����
			if (pLoader->LoadModel(ppModel, nMaxSize, nNumModel, input, line))
			{
				if (nNumModel <= nMaxSize - 1)
				{
					nNumModel++;
				}
			}

			// �p�[�c�̐ݒ肪�I����Ė���������
			if (bCharacterSet == false)
			{
				bCharacterSet = pLoader->LoadCharacterSet(ppModel, line,input);
			}

			// ���[�V�����̐ݒ�̓ǂݍ���
			pLoader->LoadMotionSet(pLoader, File, line,MAX_MOTION);

			// ���[�V�����̐����ő�܂ōs������
			if (pLoader->GetNumMotion() >= MAX_MOTION)
			{
				break;
			}
		}

		// �t�@�C�������
		File.close();

		// ���f���̑�����n��
		*OutNumModel = nNumModel;

		// �������̃R�s�[
		memcpy(&(*pInfo), pLoader->m_aInfo, sizeof((*pInfo)));

		return pLoader;
	}
	else
	{	
		// ���b�Z�[�W�{�b�N�X
		MessageBox(NULL, pFileName, "�t�@�C�����J���܂���ł���", MB_OK);

		return nullptr;
	}
}

//===================================================
// ���f���̃��[�h����
//===================================================
bool CLoderText::LoadModel(CModel** ppModel, const int nMaxSize, int nCnt, string input, string line)
{
	if (line.find("MODEL_FILENAME") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���f���̖��O�i�[�p�ϐ�
		string modelName;

		// ���l��������
		value_Input >> modelName;

		// ���f���̖��O����
		const char* MODEL_NAME = modelName.c_str();

		// �T�C�Y�ȏ�ɓǂݍ��ނƃG���[���o�邽�ߐ���
		if (nCnt <= nMaxSize - 1)
		{
			// ���f���̐���
			ppModel[nCnt] = CModel::Create(MODEL_NAME);

			return true;
		}
		else
		{
			MessageBox(NULL, MODEL_NAME, "����ȏ�ǂݍ��߂܂���", MB_OK);
		}
	}
	return false;
}

//===================================================
// �L�����N�^�[�̃��[�h����
//===================================================
bool CLoderText::LoadCharacterSet(CModel** ppModel, string line, string input)
{
	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	static int nIdx = 0;
	int nNumParts = 0;
	int nParent = 0;
	D3DXVECTOR3 offset = VEC3_NULL;

	static float fSpeed = 0.0f;
	static float fJumpHeight = 0.0f;

	if (line.find("MOVE") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> fSpeed;
	}
	if (line.find("JUMP") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> fJumpHeight;

		pPlayer->SetPlayer(fSpeed, fJumpHeight);
	}

	if (line.find("NUM_PARTS") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> nNumParts;

		// �p�[�c�̍ő吔��ݒ�
		SetNumModel(nNumParts);
	}

	if (line.find("INDEX") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> nIdx;
	}

	if (line.find("PARENT") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> nParent;

		// �e���f���̔ԍ��̎擾
		ppModel[nIdx]->SetParentID(nParent);

		if (nParent != -1)
		{// �e�����݂��Ă�����
			// �e�̃��f���̐ݒ�
			ppModel[nIdx]->SetParent(ppModel[nParent]);
		}
		else
		{// �e�����݂��Ă��Ȃ�������
			ppModel[nIdx]->SetParent(nullptr);
		}
	}

	if (line.find("POS") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffPos(offset);
	}

	if (line.find("ROT") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffRot(offset);
	}

	if (line.find("END_CHARACTERSET") != string::npos)
	{
		return true;
	}

	return false;
}

//===================================================
// ���[�V�����̃��[�h����
//===================================================
void CLoderText::LoadMotionSet(CLoderText* pLoader, ifstream& File, string nowLine, const int nNumMotion)
{
	string line, input;

	int loop = 0;
	int nKey = 0;
	int nCntModel = 0;
	int nStartEvent = 0;
	int nEndEvent = 0;

	if (nowLine.find("MOTIONSET") != string::npos)
	{
		while (getline(File, line))
		{
			int nNum = GetNumMotion();

			size_t equal_pos = line.find("="); // =�̈ʒu

			// [=] ���������߂�
			input = line.substr(equal_pos + 1);

			if (line.find("LOOP") != string::npos)
			{
				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> loop;

				// ���[�v���邩�ǂ���
				m_aInfo[nNum].bLoop = (loop == 1) ? true : false;
			}

			if (line.find("NUM_KEY") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].nNumkey;
			}

			if (line.find("NUM_EVENT") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].nNumEvent;
			}

			if (line.find("START_FRAME") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				while (value_Input >> m_aInfo[nNum].aEventFrame[nStartEvent].nStart)
				{
					nStartEvent++;
				}
			}

			if (line.find("END_FRAME") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				while (value_Input >> m_aInfo[nNum].aEventFrame[nEndEvent].nEnd)
				{
					nEndEvent++;
				}
			}

			if (line.find("FRAME") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].nFrame;
			}

			if (line.find("POS") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >>m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosX;
				value_Input >>m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosY;
				value_Input >>m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosZ;
			}
			if (line.find("ROT") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotX;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotY;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotZ;
			}

			if (line.find("END_KEY") != string::npos)
			{
				nCntModel++;

				nCntModel = Clamp(nCntModel, 0,pLoader->GetNumModel()  - 1);
			}

			if (line.find("END_KEYSET") != string::npos)
			{
				nKey++;
				nCntModel = NULL;
			}
			if (line.find("END_MOTIONSET") != string::npos)
			{
				nKey = NULL;

				if (nNum <= nNumMotion - 1)
				{
					nNum++;

					SetNumMotion(nNum);
				}
				else
				{
				}


				break;
			}
		}
	}
}

//===================================================
// input���琔�l�����o����
//===================================================
istringstream CLoderText::SetInputvalue(string input)
{
	// �擪�� = ������
	input.erase(0, input.find_first_not_of(" = "));

	// input���琔�l�����o������
	istringstream value_Input(input);

	return value_Input;
}
