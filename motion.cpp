//===================================================
//
// モーション [motion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
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
// マクロ定義
//***************************************************
#define NEUTRAL (0) // ニュートラル
#define OFFSET_FRAME (40) // フレームの初期基準

//===================================================
// コンストラクタ
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
// デストラクタ
//===================================================
CMotion::~CMotion()
{
}

//===================================================
// 初期化処理
//===================================================
void CMotion::Init(void)
{
	if (m_pLoader == nullptr)
	{
		return;
	}
	// モーションの総数
	int nNumMotion = m_pLoader->GetNumMotion();

	// モーションの総数分
	for (int nCntMotion = 0; nCntMotion < nNumMotion; nCntMotion++)
	{
		// キーの総数
		int nNumKey = m_aInfo[nCntMotion].nNumkey;

		// キーの総数分
		for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
		{
			// フレームが0以下だったら
			if (m_aInfo[nCntMotion].aKeyInfo[nCntKey].nFrame <= 0)
			{
				// 基準値を入れておく
				m_aInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = OFFSET_FRAME;
			}
		}
	}
}

//===================================================
// モーションのロード処理
//===================================================
CMotion* CMotion::Load(const char* pFileName, CModel** ppModel, int* OutNumModel, const int nMaxSize)
{
	CMotion::Info Info;

	CMotion* pMotion = new CMotion;

	if (pMotion != nullptr)
	{
		pMotion->m_pLoader = CLoderText::LoadTextFile(pFileName, ppModel, &Info, OutNumModel, nMaxSize);

		// ロードが成功していたら
		if (pMotion->m_pLoader != nullptr)
		{
			pMotion->m_pLoader->GetInfo(&pMotion->m_aInfo[0]);
		}
	}

	return pMotion;
}

//===================================================
// キー情報の設定処理
//===================================================
void CMotion::SetKey(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot,const int nIdx)
{
	// キーの設定
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fPosX = pos.x;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fPosY = pos.y;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fPosZ = pos.z;

	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fRotX = rot.x;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fRotY = rot.y;
	m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].aKey[nIdx].fRotZ = rot.z;
}

//===================================================
// セーブ処理(テキストファイル)
//===================================================
void CMotion::SaveDataTxt(const char *pFileName)
{
	// 書き出しファイルを開く
	ofstream outFile(pFileName);
	string line;

	// ファイルが開けたら
	if (outFile.is_open())
	{
		// 文字を書き出す
		outFile << "+--------------------------------------------------------+" << endl;
		outFile << "+	キー情報 [data/key_info.txt]						 +" << endl;
		outFile << "+--------------------------------------------------------+" << endl;

		outFile << "+--------------------------------------------------------+" << endl;
		outFile << " モーションタイプ[]" << endl;
		outFile << "+--------------------------------------------------------+" << endl;

		outFile << "MOTIONSET\n";
		outFile << "	LOOP = " << m_aInfo[m_nSelectMotion].bLoop << "			# ループするかどうか[0:ループしない / 1:ループする]\n";
		outFile << "	NUM_KEY = " << m_aInfo[m_nSelectMotion].nNumkey << "			# キーの総数\n\n";

		// イベントフレームの総数
		int nNumEvent = m_aInfo[m_nSelectMotion].nNumEvent;

		// イベントフレームがあるなら
		if (nNumEvent > 0)
		{
			outFile << " \tNUM_EVENT = " << nNumEvent << "\t\t\t\t# イベントフレームの総数\n";

			outFile << " \tSTART_FRAME = ";

			// イベントフレームの書き出し
			for (int nCnt = 0; nCnt < nNumEvent; nCnt++)
			{
				outFile << m_aInfo[m_nSelectMotion].aEventFrame[nCnt].nStart << " ";
			}

			outFile << " \t\t# 開始フレーム << 左から0,1,2... >>\n";

			outFile << "\tEND_FRAME = ";

			// イベントフレームの書き出し
			for (int nCnt = 0; nCnt < nNumEvent; nCnt++)
			{
				outFile << m_aInfo[m_nSelectMotion].aEventFrame[nCnt].nEnd << " ";
			}

			outFile << "\t\t# 終了フレーム\n\n";
		}
		// キーの総数
		int nNumkey = m_aInfo[m_nSelectMotion].nNumkey;

		// キーの総数分回す
		for (int nCnt = 0; nCnt < nNumkey; nCnt++)
		{
			// フレーム数
			int nFrame = m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].nFrame;

			// 文字を書き出す
			outFile << "\tKEYSET\t\t\t\t# --- << KEY : " << nCnt << " / " << nNumkey << " >> --- \n";
			outFile << "\t\tFRAME = " << nFrame << endl;

			// モデルの総数
			int nNumModel = m_pLoader->GetNumModel();

			// モデルの数分回す
			for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
			{
				// 文字の書き出し
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

				// 最後以外は改行
				if (nCntModel < nNumModel - 1)
				{
					outFile << "\n";
				}
			}

			outFile << "\tEND_KEYSET\n";

			// 最後以外は改行
			if (nCnt < nNumkey - 1)
			{
				outFile << "\n";
			}
		}

		outFile << "END_MOTIONSET\n";

		// ファイルを閉じる
		outFile.close();
	}
	else
	{
		MessageBox(NULL, pFileName, "ファイルが開けませんでした", MB_OK);
		return;
	}
}

//===================================================
// モーションのセーブ処理
//===================================================
void CMotion::SaveDataJson(const char* pFileName)
{
	json config;

	std::ofstream file("data/config.json");

	std::map<std::string, json> keyset;

	// jsonファイルの書き込み
	keyset["MOTIONTYPE"].push_back(m_nSelectMotion);
	keyset["LOOP"].push_back(m_aInfo[m_nSelectMotion].bLoop);

	// キーの総数を保存
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
		file << config.dump(4); // インデント付きで保存

		file.close();
	}
	//
	//std::fstream File("data/config.json");

	//bool bLoop = config["Loop"];
	//int nNumKey = config["NumKey"];

	//File.close();

}

//===================================================
// イベントフレームの判定
//===================================================
bool CMotion::GetEventFrame(const int motiontype)
{
	// 種類が違ったら
	if (m_nType != motiontype) return false;

	// イベントの総数分回す
	for (int nCnt = 0; nCnt < m_aInfo[motiontype].nNumEvent; nCnt++)
	{
		// 判定がはじまったら
		if (m_nAllCount >= m_aInfo[motiontype].aEventFrame[nCnt].nStart && m_nAllCount <= m_aInfo[motiontype].aEventFrame[nCnt].nEnd)
		{
			return true;
		}
	}
	return false;
}

//===================================================
// 終了処理
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
// 最初のモーションのブレンドが終わったら
//===================================================
void CMotion::FinishFirstBlend(void)
{
	// モーションの出だしのブレンドが終了した
	if (m_bFirst == true && m_nCounterBlend >= m_nFrameBlend)
	{
		// もとに戻す
		m_bFirst = false;

		m_nKey = 0;

		// モーションをブレンドしたモーションにする
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
// 現在のモーションの更新処理
//===================================================
void CMotion::UpdateCurrentMotion(CModel** pModel, int nIdx)
{
	// 次のキーのアドレスの取得
	Key* pKeyNext = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// 現在のキーのアドレスの取得
	Key* pKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// キー情報のアドレスの取得
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	// 次のキーまでの距離を求める
	float fDiffPosX = pKeyNext->fPosX - pKey->fPosX;
	float fDiffPosY = pKeyNext->fPosY - pKey->fPosY;
	float fDiffPosZ = pKeyNext->fPosZ - pKey->fPosZ;

	float fDiffRotX = pKeyNext->fRotX - pKey->fRotX;
	float fDiffRotY = pKeyNext->fRotY - pKey->fRotY;
	float fDiffRotZ = pKeyNext->fRotZ - pKey->fRotZ;

	// 角度の正規化
	NormalizeRot(&fDiffRotX);
	NormalizeRot(&fDiffRotY);
	NormalizeRot(&fDiffRotZ);

	// フレームと最大フレームの割合を求める
	float fRateFrame = (float)m_nCount / (float)pKeyInfo->nFrame;

	// 次のキーの位置までフレームに応じた位置を設定する
	float fPosX = LerpDiff(pKey->fPosX, fDiffPosX, fRateFrame);
	float fPosY = LerpDiff(pKey->fPosY, fDiffPosY, fRateFrame);
	float fPosZ = LerpDiff(pKey->fPosZ, fDiffPosZ, fRateFrame);

	// 次のキーの向きまでフレームに応じた向きを設定する
	float fRotX = LerpDiff(pKey->fRotX, fDiffRotX, fRateFrame);
	float fRotY = LerpDiff(pKey->fRotY, fDiffRotY, fRateFrame);
	float fRotZ = LerpDiff(pKey->fRotZ, fDiffRotZ, fRateFrame);

	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// ブレンドのモーションの更新処理
//===================================================
void CMotion::UpdateBlendMotion(CModel** pModel, int nIdx)
{
	// 現在のキーのアドレスを取得
	Key* pCurrentKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// 次のキーのアドレスを取得
	Key* pNextKey = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// キー情報のアドレスの取得
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	float fRate = (float)m_nCount / (float)pKeyInfo->nFrame; // 相対値

	float fRateBlendMotion = (float)m_nCounterBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame; // 相対値

	float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;

	// 現在のモーションの角度の差分
	float fDiffRX = pNextKey->fRotX - pCurrentKey->fRotX;
	float fDiffRY = pNextKey->fRotY - pCurrentKey->fRotY;
	float fDiffRZ = pNextKey->fRotZ - pCurrentKey->fRotZ;

	//// 角度の正規化
	//NormalizeRot(&fDiffRX);
	//NormalizeRot(&fDiffRY);
	//NormalizeRot(&fDiffRZ);

	// 現在のモーションのフレームに応じた角度を設定
	float fRotXCurrent = pCurrentKey->fRotX + fDiffRX * fRate;
	float fRotYCurrent = pCurrentKey->fRotY + fDiffRY * fRate;
	float fRotZCurrent = pCurrentKey->fRotZ + fDiffRZ * fRate;

	// ブレンドのキーのアドレスを取得
	Key* pCurrentBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nIdx];

	// ブレンドの次のキーのアドレスを取得
	Key* pNextBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nNextKeyBlend].aKey[nIdx];

	//ブレンドモーションの角度の差分を求める
	float fDiffBlendRX = pNextBlendKey->fRotX - pCurrentBlendKey->fRotX;
	float fDiffBlendRY = pNextBlendKey->fRotY - pCurrentBlendKey->fRotY;
	float fDiffBlendRZ = pNextBlendKey->fRotZ - pCurrentBlendKey->fRotZ;

	//// 角度の正規化
	//NormalizeRot(&fDiffBlendRX);
	//NormalizeRot(&fDiffBlendRY);
	//NormalizeRot(&fDiffBlendRZ);

	// ブレンドモーションのフレームに応じた角度の設定
	float fRotXBlend = pCurrentBlendKey->fRotX + fDiffBlendRX * fRateBlendMotion;
	float fRotYBlend = pCurrentBlendKey->fRotY + fDiffBlendRY * fRateBlendMotion;
	float fRotZBlend = pCurrentBlendKey->fRotZ + fDiffBlendRZ * fRateBlendMotion;

	//// 角度の正規化
	//NormalizeRot(&fRotXBlend);
	//NormalizeRot(&fRotYBlend);
	//NormalizeRot(&fRotZBlend);

	//現在のモーションの位置の差分を求める
	float fDiffPX = pNextKey->fPosX - pCurrentKey->fPosX;
	float fDiffPY = pNextKey->fPosY - pCurrentKey->fPosY;
	float fDiffPZ = pNextKey->fPosZ - pCurrentKey->fPosZ;

	// 現在のモーションのフレームに応じた位置の設定
	float fPosXCurrent = pCurrentKey->fPosX + fDiffPX * fRate;
	float fPosYCurrent = pCurrentKey->fPosY + fDiffPY * fRate;
	float fPosZCurrent = pCurrentKey->fPosZ + fDiffPZ * fRate;

	// ブレンドモーションの位置の差分を求める
	float fDiffBlendPX = pNextBlendKey->fPosX - pCurrentBlendKey->fPosX;
	float fDiffBlendPY = pNextBlendKey->fPosY - pCurrentBlendKey->fPosY;
	float fDiffBlendPZ = pNextBlendKey->fPosZ - pCurrentBlendKey->fPosZ;

	// ブレンドモーションのフレームに応じた位置の設定
	float fPosXBlend = pCurrentBlendKey->fPosX + fDiffBlendPX * fRateBlendMotion;
	float fPosYBlend = pCurrentBlendKey->fPosY + fDiffBlendPY * fRateBlendMotion;
	float fPosZBlend = pCurrentBlendKey->fPosZ + fDiffBlendPZ * fRateBlendMotion;

	// 現在の角度とブレンドする角度の差分を求める
	fDiffBlendRX = fRotXBlend - fRotXCurrent;
	fDiffBlendRY = fRotYBlend - fRotYCurrent;
	fDiffBlendRZ = fRotZBlend - fRotZCurrent;

	// 角度の正規化
	NormalizeRot(&fDiffBlendRX);
	NormalizeRot(&fDiffBlendRY);
	NormalizeRot(&fDiffBlendRZ);

	// ブレンドのフレームに応じた向きの設定
	float fRotX = fRotXCurrent + fDiffBlendRX * fRateBlend;
	float fRotY = fRotYCurrent + fDiffBlendRY * fRateBlend;
	float fRotZ = fRotZCurrent + fDiffBlendRZ * fRateBlend;

	// 現在の位置とブレンドする位置の差分を求める
	fDiffBlendPX = fPosXBlend - fPosXCurrent;
	fDiffBlendPY = fPosYBlend - fPosYCurrent;
	fDiffBlendPZ = fPosZBlend - fPosZCurrent;

	// ブレンドのフレームに応じた位置の設定
	float fPosX = fPosXCurrent + fDiffBlendPX * fRateBlend;
	float fPosY = fPosYCurrent + fDiffBlendPY * fRateBlend;
	float fPosZ = fPosZCurrent + fDiffBlendPZ * fRateBlend;

	// 位置を設定
	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	// 向きを設定
	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// 更新処理
//===================================================
void CMotion::Update(CModel** pModel,const int nNumModel,bool bView)
{
	for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
	{
		// キーの総数
		m_nNumKey = m_aInfo[m_nType].nNumkey;
		m_nNumKeyBlend = m_aInfo[m_nTypeBlend].nNumkey;

		// ループするかどうか
		m_bLoopMotion = m_aInfo[m_nType].bLoop;

		//if (motiontype < 0 || motiontype >= TYPE::TYPE_MAX)
		//{
		//	return;
		//}
		
		// キーの総数が0じゃないなら
		if (m_nNumKey != 0)
		{
			// のキーを増やす
			m_nextKey = (m_nKey + 1) % m_nNumKey;
		}

		// キーの総数が0じゃないなら
		if (m_nNumKeyBlend != 0)
		{
			// 次のブレンドキーを増やす
			m_nNextKeyBlend = (m_nKeyBlend + 1) % m_nNumKeyBlend;
		}

		if (m_bFinish == false && m_bFirst == false)
		{
			// 現在のモーションの更新処理
			UpdateCurrentMotion(pModel, nCntModel);
		}
		if ((m_bFinish == true || m_bFirst == true) && m_bBlend == true)
		{
			// ブレンドのモーションの更新処理
			UpdateBlendMotion(pModel, nCntModel);
		}
	}	
	
	if (bView == true && IsEndMotion())
	{
		m_bBlend = true;
	}

	// 計算用最大フレーム
	int nAllFrame = 0;

	for (int nCnt = 0; nCnt < m_nNumKey; nCnt++)
	{
		// フレームを加算
		nAllFrame += m_aInfo[m_nType].aKeyInfo[nCnt].nFrame;
	}

	// 最大フレームをセット
	m_nAllFrame = nAllFrame;

	// 最大を超えたら
	if (m_nAllCount >= m_nAllFrame)
	{
		m_nAllCount = 0;
	}

	// モーションが終了したら
	if (IsEndMotion())
	{
		m_nKeyBlend = 0;
		m_nCounterBlend = 0;
		m_nFrameBlend = m_aInfo[m_nType].aKeyInfo[m_nNumKey - 1].nFrame;
		m_bFinish = true;
		m_nTypeBlend = NEUTRAL;
	}

	// モーションの出だしのブレンドが終了した
	FinishFirstBlend();

	// キーが最大かつブレンドのカウントが最大になった
	if (IsFinishEndBlend() == true)
	{
		m_nAllCount = m_nFrameBlend;
		m_bFinish = false;			// もとに戻す
		m_bBlend = false;				// もとに戻す
		m_nType = NEUTRAL;				// モーションタイプをニュートラルにする
		m_nCount = m_nFrameBlend;
		m_nCounterBlend = 0;
	}

	// カウントがフレームを超えたら
	if (m_nCount >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		// キーを増やす
		m_nKey++;

		// 範囲内にラップする
		m_nKey = Wrap(m_nKey, 0, m_aInfo[m_nType].nNumkey - 1);

		// カウントをリセット
		m_nCount = 0;
	}

	// ブレンドカウンターがフレームを超えたら
	if (m_nCounterBlend >= m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame && (m_bFinish == true || m_bFirst == true))
	{
		// キーを増やす
		m_nKeyBlend++;

		// 範囲内にラップする
		m_nKeyBlend = Wrap(m_nKeyBlend, 0, m_aInfo[m_nTypeBlend].nNumkey - 1);
	}

	// 最初のブレンドをしていなかったら
	if (m_bFirst == false)
	{
		m_nAllCount++;

		m_nCount++;
	}

	// ブレンドが始まったら
	if (m_bFinish == true || m_bFirst == true)
	{
		m_nCounterBlend++;
	}
}

//===================================================
// モーションの設定処理
//===================================================
void CMotion::SetMotion(const int motiontype, bool bBlend, const int nBlendFrame)
{
	// 同じ種類のモーションだったら
	if (m_nTypeBlend == motiontype || m_nType == motiontype) return;

	// モーションの総数を超えていたら
	if (m_pLoader->GetNumMotion() <= motiontype)
	{
		return;
	}

	// ブレンドがあるなら
	if (bBlend == true)
	{
		m_nKeyBlend = 0;

		m_nCounterBlend = 0;		 // ブレンドカウンターをリセット
		m_nFrameBlend = nBlendFrame; // ブレンドフレームを設定する
		m_bFirst = true;			 // 最初のブレンド開始フラグをtrueにする
		
		//m_nKey = 0;
		m_bFinish = false;			 // モーションが終わっていない
		m_nTypeBlend = motiontype;   // ブレンド先の種類を設定
	}
	else
	{
		m_nKey = 0;						// 最初から始める
		m_nCount = 0;					// 最初から始める
		m_nType = motiontype;			// ブレンドするモーションのタイプを代入
		m_nTypeBlend = motiontype;		// ブレンドするモーションのタイプを代入
		m_bFinish = false;
	}

	m_bBlend = bBlend; // ブレンドするかどうか判定

	m_nAllCount = 0; // 全体のフレームのカウンターをリセットする
}

//===================================================
// モーションが終わったかどうか
//===================================================
bool CMotion::IsEndMotion(void)
{
	// モーションが終了したら
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
// 終わりのブレンドが完了したか判定
//===================================================
bool CMotion::IsFinishEndBlend(void)
{
	// ブレンドが終了したら(終わりから)
	if (m_bFinish == true && m_nFrameBlend <= m_nCounterBlend && m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// モーションのデータの設定
//===================================================
void CMotion::MotionSetElement(CImGuiManager* pImGui, CModel** ppModel, bool bViewMode)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	// キーが総数を超えていたら
	if (m_nSelectKey >= m_aInfo[m_nSelectMotion].nNumkey)
	{
		// 総数のひとつ前に戻す
		m_nSelectKey = m_aInfo[m_nSelectMotion].nNumkey - 1;
	}

	ImGui::PushItemWidth(100); // 幅100ピクセルにする

		// ボタンの説明文
	const char* RadioButton = m_aInfo[m_nSelectMotion].bLoop ? u8"ループする" : u8"ループしない";

	if (ImGui::RadioButton(RadioButton, m_aInfo[m_nSelectMotion].bLoop))
	{
		// フラグを入れ替える
		m_aInfo[m_nSelectMotion].bLoop = m_aInfo[m_nSelectMotion].bLoop ? false : true;
	}

	if (ImGui::SliderInt(u8"モーション選択", &m_nSelectMotion, 0, m_pLoader->GetNumMotion() - 1))
	{
		// ポーズのリセット
		Reset(ppModel, 0);
		m_nSelectKey = 0;
	}

	// ビューモードじゃないなら
	if (bViewMode == false)
	{
		// モーションの総数
		int nNum = m_pLoader->GetNumMotion();

		if (ImGui::SliderInt(u8"モーションの総数", &nNum, 0, MAX_MOTION))
		{
			// モーションの総数の設定
			m_pLoader->SetNumMotion(nNum);
		}
	}

	ImGui::DragInt(u8"フレーム", &m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].nFrame, 0.5f, 1, 256);

	// 計算用最大フレーム
	int nAllFrame = 0;

	for (int nCnt = 0; nCnt < m_aInfo[m_nSelectMotion].nNumkey; nCnt++)
	{
		// フレームを加算
		nAllFrame += m_aInfo[m_nSelectMotion].aKeyInfo[nCnt].nFrame;
	}

	// ビューモードなら
	if (bViewMode == true && ImGui::TreeNode(u8"フレームの設定"))
	{
		ImGui::Text(u8"イベントフレームの総数 %d / %d", m_aInfo[m_nSelectMotion].nNumEvent,MAX_EVENT);

		// 横に並べる
		ImGui::SameLine();

		ImGui::PushItemWidth(150); // 幅150ピクセルにする

		if (ImGui::Button(u8"＋") && m_aInfo[m_nSelectMotion].nNumEvent < MAX_EVENT)
		{
			// イベントフレームの総数を増やす
			m_aInfo[m_nSelectMotion].nNumEvent++;
		}
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(350); // 幅350ピクセルにする

		// 横に並べる
		ImGui::SameLine();

		if (ImGui::Button(u8"－") && m_aInfo[m_nSelectMotion].nNumEvent >= 1)
		{
			// イベントフレームの総数を減らす
			m_aInfo[m_nSelectMotion].nNumEvent--;
		}
		// ボタンの大きさの設定
		ImGui::PopItemWidth();

		// イベントフレームがあるなら
		if (m_aInfo[m_nSelectMotion].nNumEvent > 0)
		{
			ImGui::SliderInt(u8"イベントフレームの選択", &m_nIdxEvent, 0, m_aInfo[m_nSelectMotion].nNumEvent - 1);
		}

		// イベントフレームの総数が0より上なら
		if (m_aInfo[m_nSelectMotion].nNumEvent > 0)
		{
			// 情報の初期化
			InitInfo();

			// イベントフレームのインデックス
			int nIdx = m_nIdxEvent;

			// モーションの情報のアドレスの取得
			EventFrame* pEvent = &m_aInfo[m_nSelectMotion].aEventFrame[nIdx];

			// イベントフレームの設定
			ImGui::SliderInt(u8"開始フレームの設定", &pEvent->nStart, 1, nAllFrame);
			ImGui::SliderInt(u8"終了フレームの設定", &pEvent->nEnd, pEvent->nStart, nAllFrame);

			// モーション情報の表示
			ImGui::Text(u8"KEY     [ %d / %d ]", m_nKey, m_aInfo[m_nType].nNumkey);
			ImGui::Text(u8"FRAME     [ %d / %d ]", m_nCount, m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame);
			ImGui::Text(u8"ALLFRAME     [ %d / %d ]", m_nAllCount, m_nAllFrame);
		}

		ImGui::TreePop();
	}

	// ビューモードじゃないなら
	if (bViewMode == false)
	{
		// キーの総数の変更処理
		ImGui::DragInt(u8"##キーの総数の変更", &m_aInfo[m_nSelectMotion].nNumkey, 1.0f, 1, MAX_KEY - 1);

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

		ImGui::Text(u8"キーの総数の変更");
	}

	// キーの変更処理
	if (ImGui::SliderInt(u8"キーの変更", &m_nSelectKey, 0, m_aInfo[m_nSelectMotion].nNumkey - 1))
	{
		// ポーズのリセット
		Reset(ppModel, m_nSelectKey);

		// フレームの初期設定
		InitInfo();

		m_nKey = m_nSelectKey;
	}
	
	if (bViewMode == true)
	{
		if (ImGui::Button(u8"モーションの再生"))
		{
			// ビューの開始
			pPlayer->StartMotionView();
			SetMotion(m_nSelectMotion, true, 5);
		}
	}

	// ボタンの大きさの設定
	ImGui::PopItemWidth();
}

//===================================================
// リセット
//===================================================
void CMotion::Reset(CModel** ppModel,const int nKey)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	// モデルの総数の取得
	int nNumModel = m_pLoader->GetNumModel();

	for (int nCnt = 0; nCnt < nNumModel; nCnt++)
	{
		// モデルのキー情報のアドレスの取得
		Key* pKey = &m_aInfo[m_nSelectMotion].aKeyInfo[nKey].aKey[nCnt];

		// 位置の設定
		ppModel[nCnt]->SetPosition(D3DXVECTOR3(pKey->fPosX, pKey->fPosY, pKey->fPosZ));
		ppModel[nCnt]->SetRotaition(D3DXVECTOR3(pKey->fRotX, pKey->fRotY, pKey->fRotZ));
	}
	
	// 情報のリセット
	m_nCount = 0;
	m_nKey = nKey;
	m_nSelectKey = nKey;
	
	if (pPlayer != nullptr)
	{
		// モーションの設定
		SetMotion(m_nSelectMotion, false, 0);

		// モーションのビューの開始
		pPlayer->StartMotionView();
	}
}

//===================================================
// 情報の設定
//===================================================
void CMotion::SetInfo(Info* pInfo)
{
	memcpy(m_aInfo, pInfo, sizeof(m_aInfo));
}

//===================================================
// フレームの初期設定
//===================================================
void CMotion::InitInfo(void)
{
	// フレームが0になっていたら
	if (m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].nFrame <= 0)
	{
		// 初期値を入れておく
		m_aInfo[m_nSelectMotion].aKeyInfo[m_nSelectKey].nFrame = OFFSET_FRAME;
	}

	// キーの総数が0以下だったら
	if (m_aInfo[m_nSelectMotion].nNumkey <= 0)
	{
		// 1にする
		m_aInfo[m_nSelectMotion].nNumkey = 1;
	}

	// イベントフレームのインデックス
	int nIdx = m_nIdxEvent;

	// モーションの情報のアドレスの取得
	EventFrame* pEvent = &m_aInfo[m_nSelectMotion].aEventFrame[nIdx];

	// 開始フレームが0以下だったら
	if (pEvent->nStart <= 0)
	{
		// 開始フレームを1にする
		pEvent->nStart = 1;
	}

	// 終了フレームが0以下だったら
	if (pEvent->nEnd <= 0)
	{
		// 開始フレームを1にする
		pEvent->nEnd = 1;
	}
}

//===================================================
// コンストラクタ
//===================================================
CMotionLoader::CMotionLoader()
{
	memset(m_aInfo, NULL, sizeof(m_aInfo));
	m_nNumModel = NULL;
	m_nNumMotion = NULL;
}

//===================================================
// デストラクタ
//===================================================
CMotionLoader::~CMotionLoader()
{
}

//===================================================
// 情報の取得
//===================================================
void CMotionLoader::GetInfo(CMotion::Info* pInfo)
{
	for (int nCntMotion = 0; nCntMotion < m_nNumMotion; nCntMotion++)
	{
		memcpy(&pInfo[nCntMotion], &m_aInfo[nCntMotion], sizeof(pInfo[nCntMotion]));
	}
}

//===================================================
// テキストファイルのロード処理
//===================================================
CLoderText* CLoderText::LoadTextFile(const char* pFileName, CModel** ppModel, CMotion::Info* pInfo, int* OutNumModel,const int nMaxSize)
{
	// ファイルをロードする
	ifstream File(pFileName);
	string line;
	string input;

	int nNumModel = 0;

	D3DXVECTOR3 offset = VEC3_NULL;

	bool bCharacterSet = false;	// キャラクターの設定をしたかどうか

	if (File.is_open() == true)
	{
		// モーションを生成
		CLoderText* pLoader = new CLoderText;

		// 最後の行になるまで読み込む
		while (getline(File, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			// モデルのロード処理
			if (pLoader->LoadModel(ppModel, nMaxSize, nNumModel, input, line))
			{
				if (nNumModel <= nMaxSize - 1)
				{
					nNumModel++;
				}
			}

			// パーツの設定が終わって無かったら
			if (bCharacterSet == false)
			{
				bCharacterSet = pLoader->LoadCharacterSet(ppModel, line,input);
			}

			// モーションの設定の読み込み
			pLoader->LoadMotionSet(pLoader, File, line,MAX_MOTION);

			// モーションの数が最大まで行ったら
			if (pLoader->GetNumMotion() >= MAX_MOTION)
			{
				break;
			}
		}

		// ファイルを閉じる
		File.close();

		// モデルの総数を渡す
		*OutNumModel = nNumModel;

		// メモリのコピー
		memcpy(&(*pInfo), pLoader->m_aInfo, sizeof((*pInfo)));

		return pLoader;
	}
	else
	{	
		// メッセージボックス
		MessageBox(NULL, pFileName, "ファイルが開けませんでした", MB_OK);

		return nullptr;
	}
}

//===================================================
// モデルのロード処理
//===================================================
bool CLoderText::LoadModel(CModel** ppModel, const int nMaxSize, int nCnt, string input, string line)
{
	if (line.find("MODEL_FILENAME") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// モデルの名前格納用変数
		string modelName;

		// 数値を代入する
		value_Input >> modelName;

		// モデルの名前を代入
		const char* MODEL_NAME = modelName.c_str();

		// サイズ以上に読み込むとエラーが出るため制限
		if (nCnt <= nMaxSize - 1)
		{
			// モデルの生成
			ppModel[nCnt] = CModel::Create(MODEL_NAME);

			return true;
		}
		else
		{
			MessageBox(NULL, MODEL_NAME, "これ以上読み込めません", MB_OK);
		}
	}
	return false;
}

//===================================================
// キャラクターのロード処理
//===================================================
bool CLoderText::LoadCharacterSet(CModel** ppModel, string line, string input)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	static int nIdx = 0;
	int nNumParts = 0;
	int nParent = 0;
	D3DXVECTOR3 offset = VEC3_NULL;

	static float fSpeed = 0.0f;
	static float fJumpHeight = 0.0f;

	if (line.find("MOVE") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> fSpeed;
	}
	if (line.find("JUMP") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> fJumpHeight;

		pPlayer->SetPlayer(fSpeed, fJumpHeight);
	}

	if (line.find("NUM_PARTS") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> nNumParts;

		// パーツの最大数を設定
		SetNumModel(nNumParts);
	}

	if (line.find("INDEX") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> nIdx;
	}

	if (line.find("PARENT") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> nParent;

		// 親モデルの番号の取得
		ppModel[nIdx]->SetParentID(nParent);

		if (nParent != -1)
		{// 親が存在していたら
			// 親のモデルの設定
			ppModel[nIdx]->SetParent(ppModel[nParent]);
		}
		else
		{// 親が存在していなかったら
			ppModel[nIdx]->SetParent(nullptr);
		}
	}

	if (line.find("POS") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffPos(offset);
	}

	if (line.find("ROT") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
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
// モーションのロード処理
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

			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			if (line.find("LOOP") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> loop;

				// ループするかどうか
				m_aInfo[nNum].bLoop = (loop == 1) ? true : false;
			}

			if (line.find("NUM_KEY") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_aInfo[nNum].nNumkey;
			}

			if (line.find("NUM_EVENT") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_aInfo[nNum].nNumEvent;
			}

			if (line.find("START_FRAME") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				while (value_Input >> m_aInfo[nNum].aEventFrame[nStartEvent].nStart)
				{
					nStartEvent++;
				}
			}

			if (line.find("END_FRAME") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				while (value_Input >> m_aInfo[nNum].aEventFrame[nEndEvent].nEnd)
				{
					nEndEvent++;
				}
			}

			if (line.find("FRAME") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].nFrame;
			}

			if (line.find("POS") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >>m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosX;
				value_Input >>m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosY;
				value_Input >>m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosZ;
			}
			if (line.find("ROT") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
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
// inputから数値を取り出処理
//===================================================
istringstream CLoderText::SetInputvalue(string input)
{
	// 先頭の = を消す
	input.erase(0, input.find_first_not_of(" = "));

	// inputから数値を取り出す準備
	istringstream value_Input(input);

	return value_Input;
}
