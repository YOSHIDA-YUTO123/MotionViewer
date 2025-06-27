//===================================================
//
// オブジェクトの描画処理 [object.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"
#include "camera.h"
#include "manager.h"

//***************************************************
// 静的メンバ変数の宣言
//***************************************************
CObject* CObject::m_apObject[MAX_OBJECT][NUM_PRIORITY] = {};	// オブジェクトのポインタ
int CObject::m_nNumAll[NUM_PRIORITY] = { NULL };										// オブジェクトの総数

//===================================================
// コンストラクタ(オーバーロード)
//===================================================
CObject::CObject(int nPriority)
{
	m_nID = NULL;
	m_type = TYPE_NONE;
	m_nPriority = NULL;

	// オブジェクトの最大数分回す
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		// オブジェクトがNULLなら
		if (m_apObject[nCntObject][nPriority] == NULL)
		{
			m_apObject[nCntObject][nPriority] = this;	// 自分自身のポインタを代入
			m_nNumAll[nPriority]++;						// オブジェクトの総数をカウントアップ
			m_nID = nCntObject;							// オブジェクトのIDを渡す
			m_nPriority = nPriority;					// 優先順位
			break;
		}
	}
}
//===================================================
// デストラクタ
//===================================================
CObject::~CObject()
{
}
//===================================================
// すべてのオブジェクトの破棄
//===================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// オブジェクトの最大数分回す
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{
			// オブジェクトがNULLなら
			if (m_apObject[nCntObject][nCntPriority] != NULL)
			{
				// オブジェクトの終了処理
				m_apObject[nCntObject][nCntPriority]->Uninit();
			}
		}
	}
}
//===================================================
// すべてのオブジェクトの更新処理
//===================================================
void CObject::UpdateAll(void)
{
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// オブジェクトの数分回す
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{
			// オブジェクトがNULLなら
			if (m_apObject[nCntObject][nCntPriority] != NULL)
			{
				// すべてのオブジェクトの更新
				m_apObject[nCntObject][nCntPriority]->Update();
			}
		}
	}
}
//===================================================
// すべてのオブジェクトの描画処理
//===================================================
void CObject::DrawAll(void)
{
	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	pCamera->SetCamera();

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// オブジェクトの数分回す
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{
			// オブジェクトがNULLなら
			if (m_apObject[nCntObject][nCntPriority] != NULL)
			{
				// すべてのオブジェクトの更新
				m_apObject[nCntObject][nCntPriority]->Draw();
			}
		}
	}
}
//===================================================
// オブジェクトの取得処理
//===================================================
CObject* CObject::GetObject(const int nPriority, const int nIdx)
{
	// オブジェクトのインデックスを返す
	return m_apObject[nIdx][nPriority];
}

//===================================================
// オブジェクトの破棄
//===================================================
void CObject::Release(void)
{
	// 自分自身のIDを取得
	int nID = this->m_nID;

	// 自分の優先順位
	int nPriority = this->m_nPriority;

	// オブジェクト破棄
	if (m_apObject[nID][nPriority] != NULL)
	{
		// メモリの解放
		delete m_apObject[nID][nPriority];

		m_apObject[nID][nPriority] = NULL;

		// 総数を減らす
		m_nNumAll[nPriority]--;
	}
}
