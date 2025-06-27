//===================================================
//
// 計算 [math.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MATH_H_
#define _MATH_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

// 範囲内に収める
template <class T> T Clamp(const T value, const T min, const T max)
{
	if (value < min) return min;

	if (value > max) return max;

	return value;
};

// 範囲内にをループする
template <class T> T Wrap(const T value, const T min, const T max)
{
	if (value < min) return max;

	if (value > max) return min;

	return value;
};

// 角度を正規化する
template <class T> T NormalizeRot(T* value)
{
	if ((*value) < -D3DX_PI)
	{
		(*value) += D3DX_PI * 2.0f;
	}
	else if ((*value) >D3DX_PI)
	{
		(*value) += -D3DX_PI * 2.0f;
	}

	return (*value);
};

// 目的の角度までの距離を求め正規化する
template <class T> T NormalizeDiffRot(const T Diff, T* value)
{
	if (Diff <= -D3DX_PI)
	{
		(*value) += -D3DX_PI * 2.0f;
	}
	else if (Diff >= D3DX_PI)
	{
		(*value) += D3DX_PI * 2.0f;
	}

	return *value;
};

// 目的の値までの補間
template <class T> T LerpDest(const T dest,const T value,const float coef)
{
	T out = value + (dest - value) * coef;

	return out;
};

// 目的の値までの補間
template <class T> T LerpDiff(const T offset, const T Diff, const float fRate)
{
	T out = offset + (Diff * fRate);

	return out;
};

// 
template <class T> bool IsInRange(const T value,const T min,const T max)
{
	if (value >= min && value <= max)
	{
		return true;
	}

	return false;
};

//***************************************************
// 計算用namespaceの定義
//***************************************************
namespace math
{
	float GetDistance(const D3DXVECTOR3 diff);
	D3DXVECTOR3 GetTargetDir(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos);
	float GetTargetAngle(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPositionFromMatrix(const D3DXMATRIX mtxWorld);
	D3DXQUATERNION CreateQuaternion(D3DXQUATERNION* pOut, D3DXVECTOR3 axis, float fAngle);
	D3DXMATRIX SetMatrixRotationQuaternion(D3DXMATRIX* pOut, D3DXQUATERNION quaternion);
	D3DXVECTOR3 MatrixToEulerXYZ(const D3DXMATRIX mtxWorld);
	bool ShowSaveFileDialog(char* filePath, size_t maxLength, int* outFilterIndex);
	bool ShowOpenFileDialog(char* filePath, size_t max_length);
}
#endif