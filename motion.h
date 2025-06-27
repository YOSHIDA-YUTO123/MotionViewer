//===================================================
//
// モーション [motion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MOTION_H_
#define _MOTION_H_

//***************************************************
// インクルードファイル
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
// マクロ定義
//***************************************************
#define MAX_PARTS (32)  // パーツの最大数
#define MAX_KEY (32)    // キーの最大数
#define MAX_MOTION (15) // モーションの総数
#define MAX_EVENT (10)  // イベントフレームの総数

//***************************************************
// 前方宣言
//***************************************************
class CModel;
class CMotionLoader;

//***************************************************
// モーションクラスの定義
//***************************************************
class CMotion
{
public:

	// キーの構造体
	struct Key
	{
		float fPosX; // 位置X
		float fPosY; // 位置Y
		float fPosZ; // 位置Z

		float fRotX; // 向きX
		float fRotY; // 向きY
		float fRotZ; // 向きZ
	};

	// キー情報の構造体
	struct Key_Info
	{
		int nFrame;				// 再生フレーム
		Key aKey[MAX_PARTS];	// 各キーパーツのキー要素
	};

	// イベントフレーム構造体
	struct EventFrame
	{
		int nStart, nEnd; // 開始フレーム、終了フレーム
	};

	// モーション情報の構造体
	struct Info
	{
		bool bLoop;								// ループするかどうか
		int nNumkey;							// キーの総数
		EventFrame aEventFrame[MAX_EVENT];		// フレームの設定
		int nNumEvent;							// 総数
		Key_Info aKeyInfo[MAX_KEY];				// キー情報
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
	CMotionLoader* m_pLoader;		// ローダー
	Info m_aInfo[MAX_MOTION];		// モーション情報へのポインタ
	int m_nType;				// モーションの種類
	int m_nNumKey;					// キーの総数
	int m_nKey;						// 現在のキーNo.
	int m_nCount;				// モーションのカウンター
	int m_nextKey;					// 次のキー
	bool m_bLoopMotion;				// ループするかどうか

	bool m_bBlend;			// ブレンドがあるかどうか
	bool m_bLoopMotionBlend;		// ループするかどうか
	bool m_bFinish;			// モーションが終わったかどうか
	bool m_bFirst;			// モーションが始まったフラグ

	int m_nTypeBlend;			// ブレンドの種類
	int m_nFrameBlend;				// ブレンドのフレーム数
	int m_nCounterBlend;			// ブレンドカウンター
	int m_nNumKeyBlend;				// ブレンドモーションの最大のキー
	int m_nKeyBlend;				// ブレンドモーションの現在のキー
	int m_nNextKeyBlend;			// ブレンドモーションの次のキー
	int m_nCounterMotionBlend;		// ブレンドのカウンター

	int m_nSelectKey;				// 現在選択中のキー
	int m_nSelectMotion;			// 選択中のモーション
	int m_nIdxEvent;				// イベントフレームのインデックス

	int m_nAllFrame;				// 全体のフレーム
	int m_nAllCount;				// 全体のフレームのカウンター
};

//***************************************************
// モーションのロードクラスの定義
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
	CMotion::Info m_aInfo[MAX_MOTION]; // モーションの情報
private:
	int m_nNumModel;			// モデルの最大数
	int m_nNumMotion;			// モーションの総数
};

//***************************************************
// モーションのロードクラス(textFile)の定義
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