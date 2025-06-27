//===================================================
//
// プレイヤー [player.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "player.h"
#include "input.h"
#include "manager.h"
#include"model.h"
#include"renderer.h"
#include "impact.h"
#include"motion.h"
#include"shadow.h"
#include"math.h"
#include <filesystem>
#include <iomanip>
#include "explosion.h"

using namespace math; // 名前空間を使用する
using namespace std;

//***************************************************
// マクロ定義
//***************************************************
#define PLAYER_JUMP_HEIGHT (25.0f)  // ジャンプ量
#define MOVE_SPEED (10.5f)			// 移動速度
#define SHADOW_SIZE (25.0f)		// 影の大きさ
#define SHADOW_MAX_HEIGHT (500.0f)  // 影が見える最大の高さ
#define SHADOW_A_LEVEL (0.9f)       // 影のアルファ値のオフセット

//===================================================
// コンストラクタ
//===================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_move = VEC3_NULL;
	m_rotDest = VEC3_NULL;
	m_fSpeed = NULL;
	m_bJump = true;
	memset(m_apModel, NULL, sizeof(m_apModel));
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_bDash = false;
	m_nModelIndex = 0;
	m_bMotionView = false;
	m_fJumpHeight = NULL;
	m_nSmockModel = NULL;
	m_bViewMode = false;
}

//===================================================
// デストラクタ
//===================================================
CPlayer::~CPlayer()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CPlayer::Init(void)
{
	// 初期情報のロード
	LoadSystemIni();

	m_pShadow = CShadow::Create(VEC3_NULL, 50.0f, 50.0f, WHITE);

	if (m_pMotion != nullptr)
	{
		m_pMotion->Init();
		m_pMotion->Update(&m_apModel[0], m_nNumModel,false);
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CPlayer::Uninit(void)
{
	for (int nCnt = 0; nCnt < NUM_PARTS; nCnt++)
	{
		// モデルの破棄
		if (m_apModel[nCnt] != nullptr)
		{
			// 終了処理
			m_apModel[nCnt]->Uninit();

			delete m_apModel[nCnt];

			m_apModel[nCnt] = nullptr;
		}
	}

	// モーションの終了処理
	m_pMotion->Uninit();

	// モーションの破棄
	if (m_pMotion != nullptr)
	{
		delete m_pMotion;

		m_pMotion = nullptr;
	}

	Release();
}

//===================================================
// 更新処理
//===================================================
void CPlayer::Update(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// コントローラーの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// メッシュフィールドの取得
	CMeshField* pMesh = CManager::GetMeshField();

	// ImGuiの取得
	CImGuiManager* pImGui = CRenderer::GetImGui();

	static bool bViewMode = false;

	if (m_pMotion->GetLoad() == false)
	{
		return;
	}
	// プレイヤーのモデルの設定処理
	bViewMode = SetPlayerModelElement(pImGui);

	if (bViewMode == true)
	{
		// 移動処理
		if (pJoypad->GetJoyStickL() == true)
		{
			// パッドの移動処理
			CPlayer::MoveJoypad(pJoypad);
		}
		else
		{
			// キーボードの移動処理
			if (CPlayer::MoveKeyboard(pKeyboard))
			{
				// ダッシュモーションか歩きモーションかを判定
				int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

				// ジャンプかjumpじゃないかを判定
				int motiontype = m_bJump ? isDashMotion : MOTIONTYPE_JUMP;

				// モーションの設定
				m_pMotion->SetMotion(motiontype, true, 5);

				m_bMotionView = false;
			}
		}

		if (pKeyboard->GetPress(DIK_LSHIFT) || pJoypad->GetPress(pJoypad->JOYKEY_RIGHT_SHOULDER))
		{
			m_bDash = true;
		}
		else
		{
			m_bDash = false;
		}
	}

	// 移動量の減衰
	m_move.x += (0.0f - m_move.x) * 0.25f;
	m_move.z += (0.0f - m_move.z) * 0.25f;

	// 位置の更新
	m_pos += m_move;

	float fHeight = 0.0f;

	// メッシュフィールドの当たり判定
	if (pMesh->Collision(m_pos,&fHeight))
	{
		m_pos.y = fHeight;

		if (m_bJump == false && m_bMotionView == false)
		{
			m_pMotion->SetMotion(MOTIONTYPE_LANDING, true, 5);
			CMeshCircle::Create(m_pos, 18, 1, 10.0f, 80.0f, 10.0f, 60, D3DXCOLOR(1.0f, 1.0f, 0.5f, 1.0f));
			m_bJump = true;
		}
	}

	// 重力を加算
	m_move.y += -MAX_GLABITY;

	if (m_pMotion->GetEventFrame(MOTIONTYPE_MOVE))
	{
		// 煙の生成
		CExplosion::Create(D3DXVECTOR3(m_pos.x,m_pos.y + 5.0f,m_pos.z), D3DXVECTOR3(15.0f, 15.0f, 0.0f), WHITE, 4, 3, 5);
	}

	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 				// 地面の法線ベクトルの取得
		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 上方向ベクトルの作成

		// 地面の角度に合わせた角度を取得
		D3DXVECTOR3 rot = m_pShadow->GetFieldAngle(FieldNor, PlayerRay);

		// 影の設定処理
		m_pShadow->Setting(D3DXVECTOR3(m_pos.x, m_pos.y - fHeight, m_pos.z),D3DXVECTOR3(m_pos.x, fHeight + 2.0f, m_pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT,SHADOW_A_LEVEL);

		m_pShadow->SetRotaition(rot);
	}

	// ジャンプできるなら
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		m_pMotion->SetMotion(MOTIONTYPE_JUMP, true, 5);

		// 移動量を上方向に設定
		m_move.y = m_fJumpHeight;
		m_bJump = false;
	}

	if (pKeyboard->GetTrigger(DIK_RETURN))
	{
		m_pMotion->SetMotion(MOTIONTYPE_ACTION, true,5);
	}

	// プレイヤーのモーションの遷移
	TransitionMotion();

	if (m_pMotion != nullptr && bViewMode == true)
	{
		// モーションの更新処理
		m_pMotion->Update(&m_apModel[0], m_nNumModel,m_bMotionView);
	}

	// 向きの差分
	float fDiffRot = m_rotDest.y - m_rot.y;

	// 角度の正規化
	NormalizeDiffRot(fDiffRot, &m_rot.y);

	// 向きを目的の向きに近づける
	m_rot = LerpDest(m_rotDest, m_rot, 0.1f);
}

//===================================================
// 描画処理
//===================================================
void CPlayer::Draw(void)
{
	// レンダラーの取得
	CRenderer *pRenderer =  CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		// モデルがあったら
		if (m_apModel[nCnt] != nullptr)
		{
			if (m_bViewMode == true)
			{
				// 描画処理
				m_apModel[nCnt]->Draw();

				continue;
			}
			if (m_nModelIndex == nCnt)
			{
				// 描画処理
				m_apModel[nCnt]->Draw(0.5f);
			}
			else
			{
				// 描画処理
				m_apModel[nCnt]->Draw();
			}
		}
	}
}

//===================================================
// キーボードの移動処理
//===================================================
bool CPlayer::MoveKeyboard(CInputKeyboard* pKeyboard)
{
	bool bMove = false;

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 速さ
	float fSpeed = m_bDash ? m_fSpeed : m_fSpeed * 0.15f;

	if (pKeyboard->GetPress(DIK_A))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			m_move.x += sinf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;
			m_move.z += cosf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;

			m_rotDest.y = cameraRot.y + D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			m_move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			m_move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			m_rotDest.y = cameraRot.y + D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(左)
		else
		{
			m_move.z += sinf(cameraRot.y) * fSpeed;
			m_move.x -= cosf(cameraRot.y) * fSpeed;

			m_rotDest.y = cameraRot.y + D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(右)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W))
		{
			m_move.x += sinf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;
			m_move.z += cosf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;

			m_rotDest.y = cameraRot.y - D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			m_move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			m_move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			m_rotDest.y = cameraRot.y - D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(右)
		else
		{
			m_move.z -= sinf(cameraRot.y) * fSpeed;
			m_move.x += cosf(cameraRot.y) * fSpeed;

			m_rotDest.y = cameraRot.y - D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		m_move.x += sinf(cameraRot.y) * fSpeed;
		m_move.z += cosf(cameraRot.y) * fSpeed;

		m_rotDest.y = cameraRot.y + D3DX_PI;

		bMove = true;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		m_move.x -= sinf(cameraRot.y) * fSpeed;
		m_move.z -= cosf(cameraRot.y) * fSpeed;

		m_rotDest.y = cameraRot.y;

		bMove = true;
	}
	else
	{
		int motiontype = m_pMotion->GetBlendMotionType();

		if ((motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH) && m_bMotionView == false)
		{
			m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);
		}
	}

	return bMove;
}

//===================================================
// パッドの移動処理
//===================================================
void CPlayer::MoveJoypad(CInputJoypad* pJoypad)
{
	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 速さ
	float fSpeed = m_bDash ? MOVE_SPEED : 2.0f;

	// Lスティックの角度
	float LStickAngleY = pStick->Gamepad.sThumbLY;
	float LStickAngleX = pStick->Gamepad.sThumbLX;

	// デッドゾーン
	float deadzone = 32767.0f * 0.25f;

	// スティックの傾けた角度を求める
	float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

	// 動かせる
	if (magnitude > deadzone)
	{
		// アングルを正規化
		float normalizeX = (LStickAngleX / magnitude);
		float normalizeY = (LStickAngleY / magnitude);

		// プレイヤーの移動量
		float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
		float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

		// 移動量をスティックの角度によって変更
		float speedWk = fSpeed * ((magnitude - deadzone) / (32767.0f - deadzone));

		// プレイヤーの移動
		m_move.x += moveX * speedWk;
		m_move.z += moveZ * speedWk;

		// プレイヤーの角度を移動方向にする
		m_rotDest.y = atan2f(-moveX, -moveZ);

		// ダッシュモーションか歩きモーションかを判定
		int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

		// ジャンプかjumpじゃないかを判定
		int motiontype = m_bJump ? isDashMotion : MOTIONTYPE_JUMP;

		m_pMotion->SetMotion(motiontype, true, 5);
	}
	else
	{
		// モーションの種類を取得
		int motiontype = m_pMotion->GetBlendMotionType();

		// 移動モーションだったらニュートラルに戻す
		if (motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH)
		{
			m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);
		}
	}
	
}

//===================================================
// プレイヤーの要素の設定処理
//===================================================
bool CPlayer::SetPlayerModelElement(CImGuiManager *pImGui)
{
	// 位置,大きさの設定
	pImGui->SetPosition(ImVec2(0.0f, 0.0f));
	pImGui->SetSize(ImVec2(400.0f, 500.0f));

	// 描画開始
	pImGui->Start("motion", CImGuiManager::TYPE_DEFOULT);

	// モデルが読み込まれて無かったら
	if (m_pMotion->GetLoad() == false)
	{
		ImGui::Text(u8"読み込みに失敗しました。[ エラー ]");

		pImGui->End();

		return false;
	}

	//ImGui::ShowStyleEditor();

	D3DXVECTOR3 Angle = m_apModel[m_nModelIndex]->GetRotaition();
	D3DXVECTOR3 pos = m_apModel[m_nModelIndex]->GetPosition();;

	if (ImGui::RadioButton(u8"ビューモード", m_bViewMode))
	{
		m_bViewMode = m_bViewMode ? false : true;

		if (m_bViewMode == false)
		{
			// モデルの位置のリセット
			m_pMotion->Reset(&m_apModel[0],0);
		}
	}

	ImGui::SameLine();

	const char* cMode = m_bViewMode ? "ON" : "OFF";

	ImGui::Text(u8"[ %s ]", cMode);

	// モーションの情報の設定
	m_pMotion->MotionSetElement(pImGui, &m_apModel[0], m_bViewMode);

	if (m_bViewMode == true)
	{
		ImGui::PushItemWidth(150); // 幅250ピクセルにする

		// 目的の注視点
		D3DXVECTOR3 posRDest;

		// 注視点を算出
		posRDest.x = m_pos.x + sinf(m_rotDest.y) * 1.0f;
		posRDest.y = m_pos.y + sinf(m_rotDest.y) * 1.0f;
		posRDest.z = m_pos.z + cosf(m_rotDest.y) * 1.0f;

		// カメラの取得処理
		CCamera* pCamera = CManager::GetCamera();

		// カメラ追従するかどうか
		static bool bTracking = false;

		if (ImGui::Button(bTracking ? u8"カメラ追従 [ ON ] " : u8"カメラ追従 [ OFF ]"))
		{
			// フラグの入れ替え
			bTracking = bTracking ? false : true;
		}

		// 追従出来たら
		if (bTracking == true)
		{
			// カメラの追従処理
			pCamera->SetTracking(posRDest, 1.0f, 0.1f);
		}

		ImGui::DragFloat(u8"プレイヤーの移動量", &m_fSpeed, 0.1f, 0.5f, 100.0f);

		ImGui::DragFloat(u8"プレイヤーのジャンプ量", &m_fJumpHeight, 0.1f, 0.5f, 100.0f);

		ImGui::PopItemWidth();

	}
	else
	{
		// 無効化するブロック
		if (m_bViewMode == true)
		{
			ImGui::BeginDisabled(); // これで以降のUIがグレーアウト＆操作不可になる
		}

		ImGui::PushItemWidth(100); // 幅100ピクセルにする

		// モデルのインデックスの変更
		if (ImGui::SliderInt(u8"モデル選択", &m_nModelIndex, 0, m_nNumModel - 1))
		{
			// 範囲外だったらクランプする
			m_nModelIndex = Clamp(m_nModelIndex, 0, m_nNumModel - 1);
		}

		int nParent = m_apModel[m_nModelIndex]->GetParentID();
		
		ImGui::SameLine();

		// モデルのインデックスの変更
		if (ImGui::SliderInt(u8"親モデル選択", &nParent, -1, m_nNumModel - 1))
		{
			// 範囲外だったらクランプする
			nParent = Clamp(nParent, -1, m_nNumModel - 1);

			// 親モデルのIDの設定
			m_apModel[m_nModelIndex]->SetParentID(nParent);

			// 親モデルの設定
			if (nParent != -1)
			{// 親がいる
				m_apModel[m_nModelIndex]->SetParent(m_apModel[nParent]);
			}
			else
			{
				// 親がいない
				m_apModel[m_nModelIndex]->SetParent(nullptr);
			}
		}
		ImGui::Spacing();  // 少しだけ縦にスペースを入れる

		ImGui::PopItemWidth();

		ImGui::PushItemWidth(250); // 幅250ピクセルにする

		// スライダー
		if (ImGui::DragFloat3(u8"モデルの位置XYZ", pos, 1.0f))
		{
			// 位置の設定処理
			m_apModel[m_nModelIndex]->SetPosition(pos);
		}
		//ImGui::NewLine();  // 1行分のスペース（改行）
		ImGui::Spacing();  // 少しだけ縦にスペースを入れる
		//ImGui::Dummy(ImVec2(0.0f, 5.0f));  // 高さ10ピクセルの空白

		// スライダー
		if (ImGui::DragFloat3(u8"モデルの向きXYZ", Angle,0.05f, -D3DX_PI, D3DX_PI, "%.2f"))
		{
			// 向きの設定処理
			m_apModel[m_nModelIndex]->SetRotaition(Angle);
		}

		// ヒントの表示
		if (ImGui::IsItemHovered() && !ImGui::IsItemActive())
		{
			ImGui::SetTooltip(u8"Ctrlを押しながらクリックで入力可能");
		}

		// オフセットの設定
		if (ImGui::TreeNode(u8"オフセット設定"))
		{
			D3DXVECTOR3 offpos = m_apModel[m_nModelIndex]->GetOffPos();
			D3DXVECTOR3 offrot = m_apModel[m_nModelIndex]->GetOffRot();

			// スライダー
			if (ImGui::DragFloat3(u8"モデルの位置XYZ", offpos, 1.0f))
			{
				// 位置の設定処理
				m_apModel[m_nModelIndex]->SetOffPos(offpos);
			}
			// スライダー
			if (ImGui::SliderFloat3(u8"モデルの向きXYZ", offrot, -D3DX_PI, D3DX_PI, "%.2f"))
			{
				// 向きの設定処理
				m_apModel[m_nModelIndex]->SetOffRot(offrot);
			}

			ImGui::TreePop();
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));       // 通常状態

		if (ImGui::Button(u8"ポーズのリセット"))
		{
			// リセット確認
			int nID = MessageBox(NULL, "本当にリセットしますか？", "リセット", MB_YESNO);

			// ハイを押したら
			if (nID == IDYES)
			{
				// すべてのモデルをリセットする
				for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
				{
					m_apModel[nCnt]->SetPosition(VEC3_NULL);
					m_apModel[nCnt]->SetRotaition(VEC3_NULL);
				}
			}
		}

		ImGui::PopStyleColor(1);  // 3個 Push したので3個 Pop

		ImGui::PopItemWidth();

		if (ImGui::Button(u8"ポーズのコピー"))
		{
			for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
			{
				// モデルの位置の取得
				D3DXVECTOR3 modelpos = m_apModel[nCnt]->GetPosition();
				D3DXVECTOR3 modelrot = m_apModel[nCnt]->GetRotaition();

				// ポーズのセーブ
				m_apModel[nCnt]->SavePose(modelpos, modelrot);
			}
		}

		ImGui::SameLine(); // 横に並べる

		if (ImGui::Button(u8"ポーズのペースト"))
		{
			for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
			{
				// ポーズの設定
				m_apModel[nCnt]->SetPose();
			}
		}

		ImGui::SameLine(); // 横に並べる

		if (ImGui::Button(u8"ポーズの登録"))
		{
			for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
			{
				// 位置の取得
				D3DXVECTOR3 modelpos = m_apModel[nCnt]->GetPosition();
				D3DXVECTOR3 modelrot = m_apModel[nCnt]->GetRotaition();

				// キーの設定
				m_pMotion->SetKey(modelpos, modelrot,nCnt);
			}
		}

		char filePath[MAX_PATH] = { "data/key_info.txt" };  // 保存するファイルのパスを格納

		if (ImGui::Button(u8"モーションのセーブ"))
		{
			int fiter = 0;
			// ファイル保存ダイアログを表示
			if (ShowSaveFileDialog(filePath, MAX_PATH,&fiter))
			{
				switch (fiter)
				{
				case 1:
					// モーションのデータの書き出し
					m_pMotion->SaveDataTxt(filePath);
					break;
				case 2:
					break;
				default:
					break;
				}
			}
		}

		if (ImGui::Button(u8"オフセットのセーブ"))
		{
			int fiter = 0;
			// ファイル保存ダイアログを表示
			if (ShowSaveFileDialog(filePath, MAX_PATH, &fiter))
			{
				switch (fiter)
				{
				case 1:
					// モーションのデータの書き出し
					SaveOffset(filePath);
					break;
				case 2:
					break;
				default:
					break;
				}
			}
		}

		//if (ImGui::Button("json"))
		//{
		//	m_pMotion->SaveDataJson(NULL);
		//}


		if (m_bViewMode == true)
		{
			ImGui::EndDisabled(); // 無効化を終了
		}
	}
	pImGui->End();

	return m_bViewMode;
}

//===================================================
// プレイヤーのオフセットのセーブ
//===================================================
void CPlayer::SaveOffset(const char *pFileName)
{
	// 書き出しファイルを開く
	ofstream outFile(pFileName);
	string line;

	// ファイルが開けたら
	if (outFile.is_open())
	{
		// 文字を書き出す
		outFile << "#--------------------------------------------------------\n";
		outFile << "# キャラクター情報\n";
		outFile << "#--------------------------------------------------------\n";

		outFile << "CHARACTERSET\n";
		outFile << "\tNUM_PARTS = " << m_nNumModel << "\t\t# パーツの総数\n";

		// 少数点以下を書き込む
		outFile << std::fixed << std::setprecision(2);

		outFile << "\tMOVE = " << m_fSpeed << "\t\t# 移動量\n";

		outFile << "\tJUMP = " << m_fJumpHeight << "\t\t# ジャンプ量\n";

		// もとに戻す
		outFile << std::defaultfloat;

		outFile << "\n";

		// モデルの情報の保存
		for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
		{
			outFile << "\tPARTSSET\n";

			outFile << "\t\tINDEX = " << nCnt << "\t\t\t\t\t# \n";

			outFile << "\t\tPARENT = " << m_apModel[nCnt]->GetParentID() << "\t\t\t\t\t# 親モデル()\n";

			outFile << "\t\tPOS = ";
			outFile << std::fixed << std::setprecision(2);

			// オフセットを保存
			D3DXVECTOR3 offPos = m_apModel[nCnt]->GetOffPos();

			outFile << offPos.x << " ";
			outFile << offPos.y << " ";
			outFile << offPos.z << "\t\t# 位置\n";

			outFile << "\t\tROT = ";

			// オフセットを保存
			D3DXVECTOR3 offRot = m_apModel[nCnt]->GetOffRot();

			outFile << offRot.x << " ";
			outFile << offRot.y << " ";
			outFile << offRot.z << "\t\t# 向き\n";

			outFile << std::defaultfloat;

			outFile << "\tEND_PARTSSET\n\n";
		}

		outFile << "END_CHARACTERSET\n";

		// ファイルを閉じる
		outFile.close();
	}
}

//===================================================
// system.iniファイル
//===================================================
void CPlayer::LoadSystemIni(void)
{
	// ファイルを開く
	fstream file("data/system.ini");
	string line;
	string input;

	// ファイルが開けたら
	if (file.is_open())
	{
		// 最後の行になるまで読み込む
		while (getline(file, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			// FILENAMEを読み取ったら
			if (line.find("FILENAME") != string::npos)
			{
				// 先頭の = を消す
				input.erase(0, input.find_first_not_of(" = "));

				// inputから数値を取り出す準備
				istringstream value_Input(input);

				// モデルの名前格納用変数
				string FileName;

				// 数値を代入する
				value_Input >> FileName;

				// モデルの名前を代入
				const char* FILE_NAME = FileName.c_str();

				m_pMotion = CMotion::Load(FILE_NAME, &m_apModel[0], &m_nNumModel, NUM_PARTS);
			}
		}
		file.close();
	}
}

//===================================================
// プレイヤーの設定処理
//===================================================
void CPlayer::SetPlayer(const float fSpeed, const float fJumpHeight)
{
	m_fSpeed = fSpeed;
	m_fJumpHeight = fJumpHeight;
}

//===================================================
// プレイヤーのモーションの遷移
//===================================================
void CPlayer::TransitionMotion(void)
{
	// モーションの種類
	MOTIONTYPE motiontype = (MOTIONTYPE)m_pMotion->GetBlendMotionType();

	// モーションの遷移
	switch (motiontype)
	{
	case MOTIONTYPE_NEUTRAL:
		break;
	case MOTIONTYPE_MOVE:
		break;
	case MOTIONTYPE_ACTION:
		break;
	default:
		break;
	}
}


//===================================================
// 生成処理
//===================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = nullptr;

	// 3Dオブジェクトの生成
	pPlayer = new CPlayer;

	// 優先順位の取得
	int nPriority = pPlayer->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pPlayer != nullptr)
	{
		// 自分のポインタの解放
		pPlayer->Release();

		// nullにする
		pPlayer = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pPlayer == nullptr) return nullptr;

	pPlayer->m_pos = pos;
	pPlayer->m_rot = rot;

	return pPlayer;
}
