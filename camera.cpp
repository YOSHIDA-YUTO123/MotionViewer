//===================================================
//
// カメラ [camera.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "camera.h"
#include "object.h"
#include"manager.h"
#include"renderer.h"
#include "player.h"
#include"math.h"

//***************************************************
// マクロ定義
//***************************************************
#define MAX_VIEWUP (2.90f) // カメラの制限
#define MAX_VIEWDOWN (0.1f) // カメラの制限

//===================================================
// コンストラクタ
//===================================================
CCamera::CCamera()
{
	memset(m_mtxView, NULL, sizeof(D3DXMATRIX));
	memset(m_mtxProjection, NULL, sizeof(D3DXMATRIX));
	m_posR = VEC3_NULL;
	m_posV = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_posRDest = VEC3_NULL;
	m_posVDest = VEC3_NULL;
	m_vecU = VEC3_NULL;
	m_fDistance = NULL;
	m_state = STATE_NONE;
}

//===================================================
// デストラクタ
//===================================================
CCamera::~CCamera()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CCamera::Init(void)
{
	m_posV = D3DXVECTOR3(0.0f, 250.0f, -250.0f);		// 視点

	m_rot = D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f);	// 角度

	float fRotX = m_posV.x - m_posR.x;
	float fRotY = m_posV.y - m_posR.y;
	float fRotZ = m_posV.z - m_posR.z;

	// 距離を求める
	m_fDistance = sqrtf((fRotX * fRotX) + (fRotY * fRotY) + (fRotZ * fRotZ));

	//カメラの注視点初期座標を設定
	m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル

	m_state = STATE_TRACKING;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CCamera::Uninit(void)
{
}

//===================================================
// 更新処理
//===================================================
void CCamera::Update(void)
{
	// マウスの視点移動
	MouseView();
#if 0

	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	// プレイヤーの取得
	D3DXVECTOR3 playerPos(pPlayer->GetPosition().x, pPlayer->GetPosition().y + 200.0f, pPlayer->GetPosition().z);
	D3DXVECTOR3 playerRot = pPlayer->GetRotation();

	m_posRDest.x = playerPos.x + sinf(playerRot.y) * 1.0f;
	m_posRDest.y = playerPos.y + sinf(playerRot.y) * 1.0f;
	m_posRDest.z = playerPos.z + cosf(playerRot.y) * 1.0f;

	m_posVDest.x = playerPos.x - sinf(m_rot.y) * m_fDistance;
	m_posVDest.y = playerPos.y - cosf(m_rot.y) * m_fDistance;
	m_posVDest.z = playerPos.z - cosf(m_rot.y) * m_fDistance;

	m_posR.x += ((m_posRDest.x - m_posR.x) * 0.07f);
	m_posR.y += ((m_posRDest.y - m_posR.y) * 0.07f);
	m_posR.z += ((m_posRDest.z - m_posR.z) * 0.07f);

	m_posV.x += ((m_posVDest.x - m_posV.x) * 0.07f);
	m_posV.z += ((m_posVDest.z - m_posV.z) * 0.07f);

#endif

	// 角度の正規化
	NormalizeRot(&m_rot.x);
	NormalizeRot(&m_rot.y);
	NormalizeRot(&m_rot.z);
}

//===================================================
// 設定,描画処理
//===================================================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(45.0f),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		100000.0f);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}

//===================================================
// マウスホイールの処理
//===================================================
void CCamera::MouseWheel(void)
{
	CInputMouse* pMouse = CManager::GetInputMouse();

	// マウスホイールの回転量を取得
	int nWheel = pMouse->GetMouseWeel();

	if (nWheel == 0)
	{
		return;
	}

	if (nWheel < 0)
	{
		m_fDistance += 100.0f;
	}
	else if (nWheel > 0)
	{
		m_fDistance -= 100.0f;
	}

	m_fDistance = Clamp(m_fDistance, 0.0f, 999999.0f);

	// カメラの視点の情報
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
}

//===================================================
// マウスの視点移動処理
//===================================================
void CCamera::MouseView(void)
{
	CCamera::MouseWheel();

	if (m_state == STATE_ROCKON) return;

	// キーボードの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetPress(DIK_LALT) == false) return;

	// マウスの取得
	CInputMouse* pMouse = CManager::GetInputMouse();

	D3DXVECTOR2 Move = pMouse->GetVelocity();
	D3DXVECTOR2 MoveOld = pMouse->GetOldVelocity();

	D3DXVECTOR2 fAngle = Move - MoveOld;


	CCamera::PadView();

	if (pMouse->OnMousePress(1))
	{
		//回転量を更新
		m_rot.y += fAngle.x * 0.01f;
		m_rot.x += fAngle.y * 0.01f;

		//回転量を制限
		if (m_rot.x > MAX_VIEWUP)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}
		else if (m_rot.x < MAX_VIEWDOWN)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}

		//カメラ座標を更新
		m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
		m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	}
	else if (pMouse->OnMousePress(0))
	{
		//回転量を更新
		m_rot.y += fAngle.x * 0.01f;
		m_rot.x += fAngle.y * 0.01f;

		//回転量を制限
		if (m_rot.x > MAX_VIEWUP)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}
		else if (m_rot.x < MAX_VIEWDOWN)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}

		// カメラの視点の情報
		m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
		m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
	}
}

//===================================================
// パッドの視点移動処理
//===================================================
void CCamera::PadView(void)
{
	// コントローラーの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// スティック
	XINPUT_STATE* pStick = pJoypad->GetJoyStickAngle();

	if (pJoypad->GetJoyStickR() == true)
	{
		float RStickAngleY = pStick->Gamepad.sThumbRY;
		float RStickAngleX = pStick->Gamepad.sThumbRX;

		float DeadZone = 10920.0f;
		float fMag = sqrtf((RStickAngleX * RStickAngleX) + (RStickAngleY * RStickAngleY));

		if (fMag >= DeadZone)
		{
			float NormalizeX = RStickAngleX / fMag;
			float NormalizeY = RStickAngleY / fMag;

			float fAngle = fMag * 0.000003f;

			m_rot.y += NormalizeX * 0.5f * fAngle;
			m_rot.x -= NormalizeY * 0.5f * fAngle;
		}
	}

	// カメラの視点の情報
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
}

//===================================================
// カメラの追従処理
//===================================================
void CCamera::SetTracking(const D3DXVECTOR3 posRDest, const float fSpeed, const float fcoef)
{
	if (m_state != STATE_TRACKING) return;

	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	// プレイヤーの取得
	D3DXVECTOR3 playerPos(pPlayer->GetPos().x, pPlayer->GetPos().y, pPlayer->GetPos().z);
	D3DXVECTOR3 playerRot = pPlayer->GetRot();

	m_posRDest.x = posRDest.x * fSpeed;
	m_posRDest.y = posRDest.y * fSpeed;
	m_posRDest.z = posRDest.z * fSpeed;

	m_posVDest.x = playerPos.x - sinf(m_rot.y) * m_fDistance;
	m_posVDest.y = playerPos.y - cosf(m_rot.y) * m_fDistance;
	m_posVDest.z = playerPos.z - cosf(m_rot.y) * m_fDistance;

	m_posR.x += ((m_posRDest.x - m_posR.x) * fcoef);
	m_posR.y += ((m_posRDest.y - m_posR.y) * fcoef);
	m_posR.z += ((m_posRDest.z - m_posR.z) * fcoef);

	m_posV.x += ((m_posVDest.x - m_posV.x) * fcoef);
	m_posV.z += ((m_posVDest.z - m_posV.z) * fcoef);
}

//===================================================
// ロックオン処理
//===================================================
void CCamera::Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos)
{
	if (m_state != STATE_ROCKON) return;

	D3DXVECTOR3 dir = playerPos - enemyPos;

	float fAngle = atan2f(dir.x, dir.z) + D3DX_PI;

	D3DXVECTOR3 posR;
	posR.x = enemyPos.x;
	posR.y = enemyPos.y;
	posR.z = enemyPos.z;

	m_posRDest = posR;

	m_rot.y = fAngle;
		
	dir.y = 0.0f; 

	D3DXVec3Normalize(&dir, &dir);

	dir *= m_fDistance;

	dir.y = playerPos.y + 200.0f;

	m_posVDest = playerPos + dir;

	m_posR.x += ((m_posRDest.x - m_posR.x) * 0.07f);
	m_posR.y += ((m_posRDest.y - m_posR.y) * 0.07f);
	m_posR.z += ((m_posRDest.z - m_posR.z) * 0.07f);

	m_posV.x += ((m_posVDest.x - m_posV.x) * 0.07f);
	m_posV.y += ((m_posVDest.y - m_posV.y) * 0.07f);
	m_posV.z += ((m_posVDest.z - m_posV.z) * 0.07f);
}
