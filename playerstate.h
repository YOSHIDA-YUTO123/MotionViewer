//================================================
//
// プレイヤーの状態 [playerstate.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include "statebase.h"

//************************************************
// 前方宣言
//************************************************
class CPlayer;

//************************************************
// プレイヤーの状態クラスの定義
//************************************************
class CPlayerState : public CStateBase
{
public:
	CPlayerState();
	virtual ~CPlayerState();
	virtual void Init(void) {};
	virtual void Update(void) {};
	virtual void Uninit(void) {};

	void SetOwner(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
protected:
	CPlayer* m_pPlayer; // プレイヤークラスへのポインタ
private:
};

//************************************************
// プレイヤーの状態ニュートラルクラスの定義
//************************************************
class CPlayerNeutral : public CPlayerState
{
public:
	CPlayerNeutral();
	~CPlayerNeutral();
	void Update(void) override;
private:
};

//************************************************
// プレイヤーの状態移動クラスの定義
//************************************************
class CPlayerMove : public CPlayerState
{
public:
	CPlayerMove();
	~CPlayerMove();
	void Update(void) override;
private:
};

#endif
