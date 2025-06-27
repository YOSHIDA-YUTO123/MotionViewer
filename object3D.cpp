//===================================================
//
// 3D�I�u�W�F�N�g�̕`�� [object3D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "object3D.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CObject3D::CObject3D()
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));
	m_pos = VEC3_NULL;
	m_pVtxBuffer = NULL;
	m_rot = VEC3_NULL;
	m_Size = VEC3_NULL;
	m_nTextureIdx = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CObject3D::~CObject3D()
{
}

//===================================================
// ����������
//===================================================
HRESULT CObject3D::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//���_�o�b�t�@�̐����E���_���̐ݒ�
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CObject3D::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// �������g�̔j��
	Release();
}

//===================================================
// �X�V����
//===================================================
void CObject3D::Update(void)
{
	//CPlayer* pPlayer = CManager::GetPlayer();

	//D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	//// ���_���̃|�C���^
	//VERTEX_3D* pVtx;

	//// ���_�o�b�t�@�̃��b�N
	//m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	//D3DXVECTOR3 nor, vec1, vec2;
	//float fHeight = 0.0f;

	//vec1 = pVtx[2].pos - pVtx[0].pos;
	//vec2 = pVtx[1].pos - pVtx[0].pos;
	//D3DXVec3Cross(&nor, &vec2, &vec1);
	//D3DXVec3Normalize(&nor, &nor);

	//D3DXVECTOR3 vec3, vec4, vec5;

	//vec3 = pVtx[0].pos - pVtx[2].pos;
	//vec4 = pVtx[1].pos - pVtx[0].pos;
	//vec5 = pVtx[2].pos - pVtx[1].pos;

	//D3DXVec3Normalize(&vec3, &vec3);
	//D3DXVec3Normalize(&vec4, &vec4);
	//D3DXVec3Normalize(&vec5, &vec5);

	//D3DXVECTOR3 vecP0, vecP1, vecP2;

	//vecP0 = PlayerPos - pVtx[2].pos;
	//vecP1 = PlayerPos - pVtx[0].pos;
	//vecP2 = PlayerPos - pVtx[1].pos;

	//D3DXVec3Normalize(&vecP0, &vecP0);
	//D3DXVec3Normalize(&vecP1, &vecP1);
	//D3DXVec3Normalize(&vecP2, &vecP2);

	//D3DXVECTOR3 cross0, cross1, cross2;

	//D3DXVec3Cross(&cross0, &vec3, &vecP0);
	//D3DXVec3Cross(&cross1, &vec4, &vecP1);
	//D3DXVec3Cross(&cross2, &vec5, &vecP2);

	//if (cross0.y >= 0.0f && cross1.y >= 0.0f && cross2.y >= 0.0f)
	//{
	//	if (nor.y != 0.0f)
	//	{
	//		float xz = ((PlayerPos.x - pVtx[0].pos.x) * nor.x + (PlayerPos.z - pVtx[0].pos.z) * nor.z);

	//		fHeight = pVtx[0].pos.y - (xz / nor.y);

	//		pPlayer->SetPosition(D3DXVECTOR3(PlayerPos.x,m_pos.y + fHeight, PlayerPos.z));
	//	}
	//}

	//// ���_�o�b�t�@�̃A�����b�N
	//m_pVtxBuffer->Unlock();
}

//===================================================
// �`�揈��
//===================================================
void CObject3D::Draw(void)
{
	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	//	���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���ݒ�
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===================================================
// �`��ɕK�v�ȏ�񂾂��̕`�揈��
//===================================================
void CObject3D::SetDraw(void)
{
	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���ݒ�
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

}

//===================================================
// ���_�̃I�t�Z�b�g�̐ݒ菈��
//===================================================
void CObject3D::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y + m_Size.y, m_pos.z + m_Size.z);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y + m_Size.y, m_pos.z + m_Size.z);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y - m_Size.y, m_pos.z - m_Size.z);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y - m_Size.y, m_pos.z - m_Size.z);

	D3DXVECTOR3 vtx0, vtx1;

	D3DXVECTOR3 nor, vec1, vec2;

	vec1 = pVtx[0].pos - pVtx[2].pos;
	vec2 = pVtx[3].pos - pVtx[2].pos;

	D3DXVec3Cross(&nor, &vec1, &vec2);
	D3DXVec3Normalize(&nor, &nor);

	// �@���x�N�g���̐ݒ�
	pVtx[0].nor = nor;
	pVtx[1].nor = nor;
	pVtx[2].nor = nor;
	pVtx[3].nor = nor;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / nPosX, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / nPosY);
	pVtx[3].tex = D3DXVECTOR2(1.0f / nPosX, 1.0f / nPosY);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �e�N�X�`����ID�̐ݒ�
//===================================================
void CObject3D::SetTextureID(const char* pTextureName)
{
	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �e�N�X�`���̖��O�̐ݒ�
	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// �ʒu�̍X�V����
//===================================================
void CObject3D::UpdatePosition(const D3DXVECTOR3 pos,const D3DXVECTOR3 Size)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	m_pos = pos;
	m_Size = Size;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-m_Size.x, m_Size.y, m_Size.z);
	pVtx[1].pos = D3DXVECTOR3(m_Size.x, m_Size.y, m_Size.z);
	pVtx[2].pos = D3DXVECTOR3(-m_Size.x, -m_Size.y, -m_Size.z);
	pVtx[3].pos = D3DXVECTOR3(m_Size.x, -m_Size.y, -m_Size.z);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �F�̍X�V����
//===================================================
void CObject3D::UpdateCol(const D3DXCOLOR col)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;
	
	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// ��������
//===================================================
CObject3D* CObject3D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 size, const char* pTextureName)
{
	CObject3D* pObject3D = nullptr;

	// 3D�I�u�W�F�N�g�̐���
	pObject3D = new CObject3D;

	// �D�揇�ʂ̎擾
	int nPriority = pObject3D->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pObject3D != nullptr)
	{
		// �������g�̔j��
		pObject3D->Release();

		// null�ɂ��Ă���
		pObject3D = nullptr;

		return nullptr;
	}

	if (pObject3D == nullptr) return nullptr;

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	pObject3D->SetPosition(pos);
	pObject3D->SetRotaition(rot);
	pObject3D->SetSize(size);
	pObject3D->Init();
	pObject3D->SetOffsetVtx();
	pObject3D->m_nTextureIdx = pTexture->Register(pTextureName);

	return pObject3D;
}
