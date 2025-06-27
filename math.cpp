//===================================================
//
// 計算 [math.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "math.h"

//===================================================
// 距離の取得
//===================================================
float math::GetDistance(const D3DXVECTOR3 diff)
{
	// 距離を二乗する
	float sqX = diff.x * diff.x;
	float sqY = diff.y * diff.y;
	float sqZ = diff.z * diff.z;
	
	// 距離を求める
	float Out = sqrtf(sqX + sqY + sqZ);
	
	return Out;
}

//===================================================
// 目的の方向ベクトルの取得
//===================================================
D3DXVECTOR3 math::GetTargetDir(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos)
{
	// 方向を求める
	D3DXVECTOR3 diff = dest - pos;

	// 正規化する
	D3DXVec3Normalize(&diff, &diff);

	return diff;
}

//===================================================
// 目的までの角度の取得
//===================================================
float math::GetTargetAngle(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos)
{
	D3DXVECTOR3 diff = dest - pos;

	float fAngle = atan2f(diff.x, diff.z);

	return fAngle;
}

//===================================================
// 位置の行列をD3DXVECTOR3型に変換
//===================================================
D3DXVECTOR3 math::GetPositionFromMatrix(const D3DXMATRIX mtxWorld)
{
	D3DXVECTOR3 pos(mtxWorld._41, mtxWorld._42, mtxWorld._43);

	return pos;
}

//===================================================
// クウォータ二オンの軸の作成処理
//===================================================
D3DXQUATERNION math::CreateQuaternion(D3DXQUATERNION* pOut, D3DXVECTOR3 axis, float fAngle)
{
	// クウォータ二オン
	D3DXQUATERNION aQuaternion;

	// 角度を半分にする(クウォータ二オンは半分の角度をもとに計算するため)
	float angle = fAngle / 2.0f;

	D3DXVECTOR3 NormalizeAxis;

	// 軸を正規化する
	D3DXVec3Normalize(&NormalizeAxis, &axis);

	// クウォータ二オンの計算
	aQuaternion.w = cosf(angle);
	aQuaternion.x = sinf(angle) * NormalizeAxis.x;
	aQuaternion.y = sinf(angle) * NormalizeAxis.y;
	aQuaternion.z = sinf(angle) * NormalizeAxis.z;

	// 計算結果を入れる
	*pOut = aQuaternion;

	return aQuaternion;
}

//===================================================
// クウォータ二オンを回転行列に変換する
//===================================================
D3DXMATRIX math::SetMatrixRotationQuaternion(D3DXMATRIX* pOut, D3DXQUATERNION quaternion)
{
	// 計算用ワールドマトリックス
	D3DXMATRIX outMatrix;

	// 行列を初期化さ
	D3DXMatrixIdentity(&outMatrix);

	// 角度の2乗
	float sqX = quaternion.x * quaternion.x;
	float sqY = quaternion.y * quaternion.y;
	float sqZ = quaternion.z * quaternion.z;
	//float sqW = quaternion.w * quaternion.w;

	// 行列_11 _21 _31の計算
	outMatrix._11 = 1 - (2 * (sqY + sqZ));
	outMatrix._21 = (2 * ((quaternion.x * quaternion.y) + (quaternion.w * quaternion.z)));
	outMatrix._31 = (2 * ((quaternion.x * quaternion.z) + (quaternion.w * quaternion.y)));

	// 行列_12 _22 _32の計算
	outMatrix._12 = 2 * ((quaternion.x * quaternion.y) - (quaternion.w * quaternion.z));
	outMatrix._22 = 1 - (2 * (sqX + sqZ));
	outMatrix._32 = (2 * ((quaternion.y * quaternion.z) + (quaternion.w * quaternion.x)));

	// 行列_13 _23 _33の計算
	outMatrix._13 = 2 * ((quaternion.x * quaternion.z) - (quaternion.w * quaternion.y));
	outMatrix._23 = 2 * ((quaternion.y * quaternion.z) - (quaternion.w * quaternion.x));
	outMatrix._33 = 1 - (2 * (sqX + sqY));

	outMatrix._14 = 0.0f;
	outMatrix._24 = 0.0f;
	outMatrix._34 = 0.0f;

	outMatrix._41 = 0.0f;
	outMatrix._42 = 0.0f;
	outMatrix._43 = 0.0f;
	outMatrix._44 = 1.0f;

	// 計算結果を代入
	*pOut = outMatrix;

	// 計算したマトリックスを返す
	return outMatrix;
}

//===================================================
// 回転行列をオイラー角に変換
//===================================================
D3DXVECTOR3 math::MatrixToEulerXYZ(const D3DXMATRIX mtxWorld)
{
	D3DXVECTOR3 rot;

	if (fabsf(mtxWorld._32) < 1.0f)
	{
		rot.x = asinf(-mtxWorld._32);
		rot.y = atan2f(mtxWorld._31,mtxWorld._33);
		rot.z = atan2f(mtxWorld._12, mtxWorld._22);
	}
	else
	{
		rot.x = (mtxWorld._32 <= -1.0f) ? D3DX_PI * 0.5f : -D3DX_PI * 0.5f;
		rot.y = atan2f(-mtxWorld._13, mtxWorld._11);
		rot.z = 0.0f;
	}

	return rot;
}

//===================================================
// ファイル保存をダイアログ表示する
//===================================================
bool math::ShowSaveFileDialog(char* filePath, size_t maxLength,int *outFilterIndex)
{
	OPENFILENAME ofn;	// ダイアログボックス構造体

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);

	ofn.hwndOwner = GetActiveWindow();

	ofn.lpstrFile = filePath;

	ofn.nMaxFile = (DWORD)maxLength;
	ofn.lpstrFilter =
		"テキストファイル (*.txt)\0*.txt\0"
		"JSONファイル (*.json)\0*.json\0"
		"すべてのファイル (*.*)\0*.*\0";

	ofn.nFilterIndex = 1;
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrDefExt = "txt";

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

	bool result = (GetSaveFileName(&ofn) == TRUE);

	if (result && outFilterIndex)
	{
		*outFilterIndex = ofn.nFilterIndex;
	}

	// ダイアログボックスを表示し、保存先ファイルパスを取得
	return result;
}

//===================================================
// ファイル読み込みをダイアログ表示する
//===================================================
bool math::ShowOpenFileDialog(char* filePath, size_t max_length)
{
	OPENFILENAME ofn;    // ダイアログボックス構造体
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = (DWORD)max_length;
	ofn.lpstrFilter = "テキストファイル\0*.txt\0すべてのファイル\0*.*\0";
	ofn.lpstrFile[0] = '\0';   // 初期化
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// ダイアログボックスを表示し、選択されたファイルパスを取得
	return GetOpenFileName(&ofn) == TRUE;
}
