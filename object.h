//===================================================
//
// オブジェクトの描画処理 [object.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// マクロ定義
//***************************************************
#define VEC3_NULL (D3DXVECTOR3(0.0f,0.0f,0.0f))		// 初期化
#define VEC2_NULL (D3DXVECTOR2(0.0f,0.0f))			// 初期化
#define WHITE (D3DXCOLOR(1.0f,1.0f,1.0f,1.0f))		// 色の初期化
#define MAX_OBJECT (256)							// オブジェクトの最大数
#define FRAME (60)									// 一フレーム
#define NUM_PRIORITY (8)							// 優先順位の数
#define MAX_WORD (1024)								// 最大の文字数
#define MAX_GLABITY (1.0f)							// 重力の大きさ

//***************************************************
// レンダラークラスの定義
//***************************************************
class CObject
{
public:

	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_PLAYER,
		TYPE_ENEMY,
		TYPE_BULLET,
		TYPE_BLOCK,
		TYPE_MAX
	}TYPE;

	CObject(int nPriority = 3);
	virtual ~CObject();

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static int GetNumObject(const int nPriority) { return m_nNumAll[nPriority]; }
	static CObject* GetObject(const int nPriority, const int nIdx);

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	int GetPriority(void) const { return m_nPriority; }
	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) const { return m_type; }
protected:
	void Release(void);
private:
	static CObject* m_apObject[MAX_OBJECT][NUM_PRIORITY];	// オブジェクトのポインタ
	static int	m_nNumAll[NUM_PRIORITY];					// オブジェクトの総数
	TYPE m_type;											// オブジェクトの種類
	int	m_nID;												// オブジェクトのID
	int m_nPriority;										// 優先順位
};
#endif