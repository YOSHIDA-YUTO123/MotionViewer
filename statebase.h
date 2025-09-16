//================================================
//
// 状態パターン基盤 [statebase.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _STATEBASE_H_
#define _STATEBASE_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"
#include<memory>

//************************************************
// 状態クラスの定義
//************************************************
class CStateBase
{
public:
	CStateBase();
	~CStateBase();
	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Uninit(void) = 0;
private:
};

//************************************************
// 状態マシーンクラスの定義
//************************************************
class CStateMachine
{
public:
	CStateMachine();
	~CStateMachine();
	void Change(std::shared_ptr<CStateBase> pNewState);
	void Update(void);
private:
	std::shared_ptr<CStateBase> m_pNowState; // 現在の状態ポインタ
};

#endif