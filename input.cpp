//================================================
//
// 入力処理 [input.cpp]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// インクルードファイル
//*************************************************
#include "input.h"

//*************************************************
// マクロ定義
//*************************************************
#define DEAD_ZONE (10000) // デッドゾーン

//*************************************************
// 静的メンバ変数の宣言
//*************************************************
LPDIRECTINPUT8 CInput::m_pInput = NULL; // DIrectInputオブジェクトへのポインタ

//===============================================
// 入力処理のコンストラクタ
//===============================================
CInput::CInput()
{
	m_pDevice = NULL;
	m_pInput = NULL;
}
//===============================================
// 入力処理のデストラクタ
//===============================================
CInput::~CInput()
{

}
//===============================================
// 入力処理の初期化処理
//===============================================
HRESULT CInput::Init(HINSTANCE hInstance)
{
	// NULLだったら
	if (m_pInput == NULL)
	{
		// DirectInputオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_pInput,
			NULL)))
		{
			// 失敗
			return E_FAIL;
		}
	}

	return S_OK;
}
//===============================================
// 入力処理の初期化処理
//===============================================
void CInput::Uninit(void)
{
	// 入力デバイス(キーボード)の破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();	// アクセス権の破棄
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// DirectInputオブジェクトの破棄
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}
//===============================================
// キーボードのコンストラクタ
//===============================================
CInputKeyboard::CInputKeyboard()
{
	// キー情報の初期化
	memset(&m_aKeyState, NULL, sizeof(m_aKeyState));

	// 前のキー情報の初期化
	memset(&m_OldKeyState, NULL, sizeof(m_OldKeyState));

	// キーの長押しカウンター
	m_nRepeatCount = 0;

	// 最初に押したかどうか
	m_bFirstPress = false;
}
//===============================================
// キーボードのデストラクタ
//===============================================
CInputKeyboard::~CInputKeyboard()
{
}
//===============================================
// キーボードの初期化処理
//===============================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の初期化処理
	CInput::Init(hInstance);

	// 入力デバイス生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードのアクセス権を獲得
	m_pDevice->Acquire();

	return S_OK;
}
//===============================================
// キーボードの終了処理
//===============================================
void CInputKeyboard::Uninit(void)
{
	// 終了処理
	CInput::Uninit();
}
//===============================================
// キーボードの更新処理
//===============================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[MAX_KEYBORAD];		// キーボードの入力情報

	for (int nCntkey = 0; nCntkey < MAX_KEYBORAD; nCntkey++)
	{
		m_OldKeyState[nCntkey] = m_aKeyState[nCntkey];	// キーボードのプレス情報を保存
	}

	//入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntkey = 0; nCntkey < MAX_KEYBORAD; nCntkey++)
		{
			m_aKeyState[nCntkey] = aKeyState[nCntkey];	// キーボードのプレス情報を保存
		}
	}
	else
	{
		m_pDevice->Acquire();			// キーボードのアクセス権を獲得
	}
}
//===============================================
// キーボードのプレス処理
//===============================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}
//===============================================
// キーボードのトリガー処理
//===============================================
bool CInputKeyboard::GetTrigger(int nkey)
{
	// 判定用フラグ
	bool Trigger = false;

	if (m_aKeyState[nkey] & 0x80 && !(m_OldKeyState[nkey] & 0x80))
	{
		Trigger = true;
	}

	return Trigger;
}
//===============================================
// キーボードのリリース処理
//===============================================
bool CInputKeyboard::GetRelease(int nKey)
{
	// 判定用フラグ
	bool Trigger = false;
	if (m_OldKeyState[nKey] & 0x80 && !(m_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}

	return Trigger;
}

//===============================================
// キーボードの継続処理(連射処理)
//===============================================
bool CInputKeyboard::GetRepeat(int nKey,const int nMaxCnt)
{
	// キーを押していたら
	if (GetPress(nKey))
	{
		if (m_bFirstPress == false)
		{
			m_bFirstPress = true;
			return true;
		}

		// キーのカウントが最大まで行ったら
		if (m_nRepeatCount >= nMaxCnt)
		{
			// カウントをリセット
			m_nRepeatCount = 0;

			// 判定開始
			return true;
		}
		else
		{
			// カウントを進める
			m_nRepeatCount++;
		}
	}
	else
	{
		// カウントをリセット
		m_nRepeatCount = 0;
		m_bFirstPress = false;
	}

	return false;
}

//===============================================
// 何かキーを押すと反応する
//===============================================
bool CInputKeyboard::GetPressAnyKey(void)
{
	// キーの最大数分回す
	for (int nCnt = 0; nCnt < MAX_KEYBORAD; nCnt++)
	{
		if (m_aKeyState[nCnt] != 0) return true;
	}
	return false;
}
//===============================================
// パッドのコンストラクタ
//===============================================
CInputJoypad::CInputJoypad()
{
	// メモリのクリア
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));

	// メモリのクリア
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	// 最初にキーを押したか判定
	m_bFirstPress = false;
	m_nRepeatCount = 0;
}
//===============================================
// パッドのデストラクタ
//===============================================
CInputJoypad::~CInputJoypad()
{
}
//===============================================
// パッドの初期化処理
//===============================================
HRESULT CInputJoypad::Init(void)
{	
	// メモリのクリア
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));

	// メモリのクリア
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	// メモリのクリア
	memset(&m_joykeyStateRelease, 0, sizeof(XINPUT_STATE));

	// メモリのクリア
	memset(&m_OldKeyState, 0, sizeof(XINPUT_STATE));

	// Xinputのステートを設定(有効化)
	XInputEnable(true);

	return S_OK;
}
//===============================================
// パッドの終了処理
//===============================================
void CInputJoypad::Uninit(void)
{
	// Xinputのステートを設定(無効化)
	XInputEnable(false);
}
//===============================================
// パッドの更新処理
//===============================================
void CInputJoypad::Update(void)
{
	XINPUT_STATE joykeyState;			// 入力情報を取得

	//ジョイパッドの状態を取得.
	if (XInputGetState(0, &joykeyState) == ERROR_SUCCESS)
	{
		WORD Button = joykeyState.Gamepad.wButtons;				// 押したときの入力情報
		WORD OldButton = m_joyKeyState.Gamepad.wButtons;		// 1F前の入力情報

		m_joyKeyStateTrigger.Gamepad.wButtons = Button & ~OldButton;
		m_joykeyStateRelease.Gamepad.wButtons = OldButton & ~Button;

		m_joyKeyState = joykeyState;							// ジョイパッドのプレス情報を保存(格納)
	}
}
//===============================================
// パッドのプレスの取得処理
//===============================================
bool CInputJoypad::GetPress(JOYKEY key)
{
	return (m_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//===============================================
// パッドのトリガーの取得処理
//===============================================
bool CInputJoypad::GetTrigger(JOYKEY key)
{
	return (m_joyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//===============================================
// パッドのリリースの取得処理
//===============================================
bool CInputJoypad::GetRelease(JOYKEY key)
{
	return (m_joykeyStateRelease.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//===============================================
// パッドのリピート処理
//===============================================
bool CInputJoypad::GetRepeat(JOYKEY key, const int nMaxCnt)
{
	// キーを押していたら
	if (GetPress(key))
	{
		if (m_bFirstPress == false)
		{
			m_bFirstPress = true;
			return true;
		}

		// キーのカウントが最大まで行ったら
		if (m_nRepeatCount >= nMaxCnt)
		{
			// カウントをリセット
			m_nRepeatCount = 0;

			// 判定開始
			return true;
		}
		else
		{
			// カウントを進める
			m_nRepeatCount++;
		}
	}
	else
	{
		// カウントをリセット
		m_nRepeatCount = 0;
		m_bFirstPress = false;
	}

	return false;

}
//===============================================
// パッドのLスティック処理
//===============================================
bool CInputJoypad::GetJoyStickL(void)
{
	//ローカル変数宣言
	bool joykey = false;

	//中心じゃなかったら
	if (m_joyKeyState.Gamepad.sThumbLX >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbLX <= -DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbLY >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbLY <= -DEAD_ZONE)
	{
		joykey = true;
	}

	return joykey;
}

//===============================================
// パッドのRスティック処理
//===============================================
bool CInputJoypad::GetJoyStickR(void)
{
	//ローカル変数宣言
	bool joykey = false;

	//中心じゃなかったら
	if (m_joyKeyState.Gamepad.sThumbRX >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbRX <= -DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbRY >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbRY <= -DEAD_ZONE)
	{
		joykey = true;
	}

	return joykey;
}

//===============================================
// パッドのスティック情報の取得処理
//===============================================
XINPUT_STATE* CInputJoypad::GetJoyStickAngle(void)
{
	return &m_joyKeyState;
}
//===============================================
// 何かボタンを押すと反応する
//===============================================
bool CInputJoypad::GetPressAnyButton(void)
{
	if (m_joyKeyState.Gamepad.wButtons != 0) return true;

	return false;
}
//===============================================
// パッドのトリガーのトリガー処理
//===============================================
bool CInputJoypad::GetTriggerTrigger(JOYKEY Trigger)
{
	static bool bRightTriggerPressed = false;  // 右トリガーが押されたかどうか
	static bool bLeftTriggerPressed = false;   // 左トリガーが押されたかどうか

	// しきい値（例えば、150以上で押されたとみなす）
	const BYTE triggerThreshold = 150;

	// ジョイパッドの状態を取得
	if (XInputGetState(0, &m_joyKeyStateTrigger) == ERROR_SUCCESS)
	{
		// 現在のトリガーの値を取得
		BYTE rightTrigger = m_joyKeyStateTrigger.Gamepad.bRightTrigger;
		BYTE leftTrigger = m_joyKeyStateTrigger.Gamepad.bLeftTrigger;

		switch (Trigger)
		{
		case CInputJoypad::JOYKEY_R2:
			// 右トリガーが押されており、まだ反応していない場合
			if (rightTrigger >= triggerThreshold && !bRightTriggerPressed)
			{
				// 右トリガーが押されたことを記録
				bRightTriggerPressed = true;

				return true;
			}
			break;
		case CInputJoypad::JOYKEY_L2:
			// 左トリガーが押されており、まだ反応していない場合
			if (leftTrigger >= triggerThreshold && !bLeftTriggerPressed)
			{
				// 左トリガーが押されたときの処理
				// ここに処理を記述

				// 左トリガーが押されたことを記録
				bLeftTriggerPressed = true;

				return true; // 左トリガーが押された場合にtrueを返す
			}
			break;
		default:
			break;
		}
	}

	// トリガーが離された場合、フラグをリセット
	if (m_joyKeyStateTrigger.Gamepad.bRightTrigger < triggerThreshold)
	{
		bRightTriggerPressed = false;
	}

	if (m_joyKeyStateTrigger.Gamepad.bLeftTrigger < triggerThreshold)
	{
		bLeftTriggerPressed = false;
	}	return false;
}
//===============================================
// パッドのトリガーのプレス処理
//===============================================
bool CInputJoypad::GetTriggerPress(JOYKEY key)
{
	// ジョイパッドの状態を取得
	XInputGetState(0, &m_joyKeyState);

	switch (key)
	{
	case CInputJoypad::JOYKEY_R2:
		if (m_joyKeyState.Gamepad.bRightTrigger) return true;
		break;
	case CInputJoypad::JOYKEY_L2:
		if (m_joyKeyState.Gamepad.bLeftTrigger) return true;
		break;
	default:
		break;
	}
	return false;
}
//===============================================
// マウスのコンストラクタ
//===============================================
CInputMouse::CInputMouse()
{
	memset(&m_CurrentState, NULL, sizeof(m_CurrentState));
		
	memset(&m_PrevState, NULL, sizeof(m_PrevState));

	m_bFirstPress = false;
	m_nRepeatCount = 0;
}
//===============================================
// マウスのデストラクタ
//===============================================
CInputMouse::~CInputMouse()
{

}
//===============================================
// マウスの初期化処理
//===============================================
HRESULT CInputMouse::Init(HWND hWnd)
{
	//// 初期化処理
	//CInput::Init(hInstance, hWnd);

	if (FAILED(m_pInput->CreateDevice
	(
		GUID_SysMouse,
		&m_pDevice,
		NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse)))
	{
		return E_FAIL;
	}

	// 協調モードの設定
	if (FAILED(m_pDevice->SetCooperativeLevel(
		hWnd,
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
	{
		return E_FAIL;
	}

	//キーボードのアクセス権を獲得
	m_pDevice->Acquire();

	return S_OK;
}
//===============================================
// マウスの終了処理
//===============================================
void CInputMouse::Uninit(void)
{
	// 終了処理
	CInput::Uninit();

	//入力デバイス(マウス)の破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();//キーボードへのアクセス権を放棄
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	//DirectInputオブジェクトの攻撃
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}
//===============================================
// マウスの更新処理
//===============================================
void CInputMouse::Update(void)
{
	m_PrevState.lX = m_CurrentState.lX;
	m_PrevState.lY = m_CurrentState.lY;

	// 更新前に最新マウス情報を保存する
	m_PrevState = m_CurrentState;

	// 最新のマウスの状態を更新
	HRESULT	hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_CurrentState);

	if (FAILED(hr))
	{
		m_pDevice->Acquire();
	}

	// マウス座標を取得する
	POINT p;
	GetCursorPos(&p);

	// スクリーン座標をクライアント座標に変換する
	ScreenToClient(FindWindowA(CLASS_NAME, WINDOW_NAME), &p);

	m_CurrentState.lX = p.x;
	m_CurrentState.lY = p.y;
}
//===============================================
// マウスのプレス処理
//===============================================
bool CInputMouse::OnMousePress(int button_type)
{
	return (m_CurrentState.rgbButtons[button_type] & 0x80) ? true : false;
}
//===============================================
// マウスのプレス処理
//===============================================
bool CInputMouse::OnMouseTriggerDown(int button_type)
{
	if (!(m_PrevState.rgbButtons[button_type] & (0x80)) &&
		m_CurrentState.rgbButtons[button_type] & (0x80))
	{
		return true;
	}
	return false;
}
//===============================================
// マウスのリリース処理
//===============================================
bool CInputMouse::OnMouseTriggerUp(int button_type)
{
	if ((m_PrevState.rgbButtons[button_type] & (0x80)) &&
		!(m_CurrentState.rgbButtons[button_type] & (0x80)))
	{
		return true;
	}
	return false;

}
//===============================================
// マウスのリピート処理
//===============================================
bool CInputMouse::OnMouseRepet(int button_type, const int nMaxCnt)
{
	// キーを押していたら
	if (OnMousePress(button_type))
	{
		if (m_bFirstPress == false)
		{
			m_bFirstPress = true;
			return true;
		}

		// キーのカウントが最大まで行ったら
		if (m_nRepeatCount >= nMaxCnt)
		{
			// カウントをリセット
			m_nRepeatCount = 0;

			// 判定開始
			return true;
		}
		else
		{
			// カウントを進める
			m_nRepeatCount++;
		}
	}
	else
	{
		// カウントをリセット
		m_nRepeatCount = 0;
		m_bFirstPress = false;
	}

	return false;
}
//===============================================
// マウスの移動量取得処理
//===============================================
D3DXVECTOR2 CInputMouse::GetVelocity(void)
{
	return D3DXVECTOR2((float)m_CurrentState.lX, (float)m_CurrentState.lY);
}
//===============================================
// マウスの過去の移動量取得処理
//===============================================
D3DXVECTOR2 CInputMouse::GetOldVelocity(void)
{
	return D3DXVECTOR2((float)m_PrevState.lX, (float)m_PrevState.lY);
}
//===============================================
// マウスのホイールの取得
//===============================================
int CInputMouse::GetMouseWeel(void)
{
	return m_CurrentState.lZ;
}
