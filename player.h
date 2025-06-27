//===================================================
//
// プレイヤー [player.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PLAYER_H_
#define _PLAYER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// マクロ定義
//***************************************************
#define NUM_PARTS (25) // モデルの最大数

//***************************************************
// 前方宣言
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CModel;
class CMotion;
class CShadow;
class CScoreLerper;
class CImGuiManager;

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CObject
{
public:

	// モーションの種類
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

	CMotion *m_pMotion;				// モーションのクラスへのポインタ
	CModel* m_apModel[NUM_PARTS];	// モデルクラスへのポインタ
	CShadow* m_pShadow;				// 影クラスへのポインタ

	D3DXVECTOR3 m_pos;				// 位置
	D3DXVECTOR3 m_rot;				// 向き
	D3DXVECTOR3 m_rotDest;			// 目的の向き
	D3DXVECTOR3 m_move;				// 移動量
	D3DXMATRIX m_mtxWorld;			// ワールドマトリックス

	float m_fSpeed;					// 移動速度
	float m_fJumpHeight;			// ジャンプ量

	int m_nModelIndex;				// モデルのインデックス
	int m_nParentIndex;				// 親モデルのインデックス
	int m_nNumModel;				// モデルの最大数
	int m_nSmockModel;				// 煙を出すモデル		
	bool m_bViewMode;				// ビューモードかどうか
	bool m_bMotionView;				// モーションのビューかどうか
	bool m_bJump;					// ジャンプできるかどうか
	bool m_bDash;					// 走ってるかどうか
};

#endif