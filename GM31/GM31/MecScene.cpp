#include "mecScene.h"
#include    <memory>
#include	<iostream>
#include	"system/collision.h"
#include "system/CDirectInput.h"

//float VALUE_MOVE_PLAYER = 0.06f;					// キー入力時の移動量
float VALUE_ROTATE_PLAYER = PI * 0.02f;				// キー入力時の回転量
float VALUE_JUMP_PLAYER = 3.80;						//プレイヤーのジャンプ力
float RATE_ROTATE_PLAYER = 0.40f;					// １フレーム当たりの回転割合
float GRAVITY = 0.068;								//重力

void MecScene::init() 
{
	// カメラ(3D)の初期化
	m_camera.UseGUI = true;
	m_camera.Init();
	m_cameraF.Init();

	// フィールドの初期化
	m_field = std::make_unique<Field>();
	m_field->Init();

	// シェーダーの初期化
	m_Sceneshader.Create(
			"shader/unlitTextureVS.hlsl",			// 頂点シェーダー
			"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー
			//"shader/NoizePS.hlsl");			// ピクセルシェーダー

	// シェーダーの初期化
	m_Monochromeshader.Create(
		"shader/unlitTextureVS.hlsl",			// 頂点シェーダー
			"shader/monochromePS.hlsl");			// ピクセルシェーダー(モノクロ)

	// シェーダーの初期化
	m_rgbSpritshader.Create(
		"shader/unlitTextureVS.hlsl",			// 頂点シェーダー
		"shader/rgbSpritPS.hlsl");			// ピクセルシェーダー(モノクロ)

	// シェーダーの初期化
	m_Noizeshader.Create(
		"shader/unlitTextureVS.hlsl",			// 頂点シェーダー
		"shader/NoizePS.hlsl");			// ピクセルシェーダー

	// スカイドームの初期化
	m_skydome = std::make_unique<Skydome>();
	m_skydome->Init();

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(CUBE_SIZE * 10, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, CUBE_SIZE * 10, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, CUBE_SIZE * 10));

	// ボックスの初期化
	m_shapecube = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
	m_shapecube2 = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

	

	// ボックスサイズの初期化
	m_boxSizes.fill(Vector3(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE));	// すべての要素をBOXWIDTH,BOXHEIGHT,BOXDEPTH;

	//デバック用GUI一式
	//// BOXのSRTの設定用
	//DebugUI::RedistDebugFunction([this]() {
	//	Debug_Box();
	//	});

	//// デバッグフリーカメラ
	//DebugUI::RedistDebugFunction([this]() {
	//	debugFreeCamera();
	//	});

	m_player.Init();
	//m_rock.Init();

	//m_shapecube_col = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

	//M_Rockはプレイヤー参照するから必須
	M_Player* pl = &m_player;

	auto rock = std::make_unique<M_Rock>();
	//オブジェクト分
	rock->SetPlayer(pl);
	m_objects.push_back(std::make_unique<M_Gun>());
	m_objects.push_back(std::make_unique<M_Gun>());
	//m_objects.push_back(std::move(rock));
	

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Init();
		//m_objects[i]->SetAdhesioing(false);
	}

	//銃を取り付けておく
	//AdhesioingObjects[2] = m_objects[0].get();
	//AdhesioingObjects[3] = m_objects[1].get();
	m_player.Conect(2, m_objects[0].get());
	m_player.Conect(3, m_objects[1].get());
	//接続の初期化
	/*for (int i = 0; i < ADHESIOINGMAX; i++) 
	{
		if (AdhesioingObjects[i]) 
		{
			AdhesioingObjects[i]->SetAdhesioing(true);
			AdhesioingObjects[i]->SetPosition(m_player.ConectPos(i));
			AdhesioingObjects[i]->Adhesioing();
		}
	}*/
	

	//敵
	Enemy.Init(pl);
	RockonEnemy = Enemy.GetEnemy();
	Enemy.AddGun(dynamic_cast<M_Gun*>(m_objects[0].get()));
	Enemy.AddGun(dynamic_cast<M_Gun*>(m_objects[1].get()));

	//プレイヤーが座標受け取れるようにする
	m_player.SetTarget(Enemy.GetEnemy()->GetPosition_P());

	//m_enemys[1]->SetPosition({ 25,7,5 });

	// 画像のUV座標
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 1.0f, 0),
		Vector2(0, 1.0f / 1.0f),
		Vector2(1.0f / 1.0f, 1.0f / 1.0f)
	};

	//ロックオンカーソル
	m_Rockon = std::make_unique<CSprite>(20, 20, "assets/texture/Rockon.png", uv);
	//画面補正
	// マテリアル生成
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, 0.5f);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;
	m_Screen = std::make_unique<CSprite>(200, 200, "assets/texture/ScreenEfect.png", uv,
		mtrl_Screen, "shader/unlitTextureVS.hlsl", "shader/NoizePS.hlsl");
	//操作方法出す
	m_Tutorial = std::make_unique<CSprite>(200, 200, "assets/texture/Tutorial.png", uv);
	
	//プレイヤーHP
	HP_Player_G = std::make_unique<CSprite>(200, 200, "assets/model/Mec/MecArm/Tex_green.png", uv);
	HP_Player_R = std::make_unique<CSprite>(200, 200, "assets/model/Mec/MecArm/Tex_red.png", uv);
	//エネミーHP
	HP_Enemy_G = std::make_unique<CSprite>(200, 200, "assets/model/Mec/MecArm/Tex_green.png", uv);
	HP_Enemy_R = std::make_unique<CSprite>(200, 200, "assets/model/Mec/MecArm/Tex_red.png", uv);

	m_Special = std::make_unique<CSprite>(200, 200, "assets/texture/UI_Yellow.png", uv);

}

void MecScene::update(uint64_t deltatime)
{

	//フェード
	if (Fade_Time != 0) { Fade_IN(deltatime);}

	//経過時間を記録
	float time = static_cast<float>(deltatime) / 1000;
	if (Specialcool != 5000) {
		Specialcool += time;
		if (Specialcool > 5000) Specialcool = 5000;
	}

	m_boxSRTs[0].pos = m_objects[0]->GetPosition();

	//座標入れる
	//m_boxSRTs_col[0].pos
	

	if (Fade_Time == 0) {
		if (UseCamera == UseCameraRockOn) PlayerMovetes();
		else PlayerMove();


		//接続の処理(重いから無効化しておく)
		PlayerAdhesion();
		PlayerShot();
	}


	m_player.Update(deltatime);

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Update(deltatime);
	}

	Enemy.Update(deltatime);

	//m_camera.SetLookat(Enemy.GetEnemy()->GetPosition());
	//カメラ中視点を直接渡すのではなく敵に向けて保管する形に変更する
	m_camera.SetEnemypos(Enemy.GetEnemy()->GetPosition());
	m_camera.Update_time(deltatime);
	m_camera.Update();
	switch (UseCamera)
	{
	case UseCameraNormal:
		campos = Vector3{ m_player.GetPosition()  - (m_player.GetForward() * 50)};
		campos.y += 20;
		m_camera.SetPosition(campos);
		break;
	case UseCameraRockOn://敵をロックオンするカメラ
		//Vector3 camForward = (m_player.GetPosition() - RockonEnemy->GetPosition());
		//campos = Vector3{ m_player.GetPosition() + (camForward) };
		//campos.y = 30;
		//m_camera.SetPosition(campos);
		
		//
		Vector3 camForward = m_player.GetPosition() - RockonEnemy->GetPosition();

		float sumAbs = fabs(camForward.x) + fabs(camForward.y) + fabs(camForward.z);
		float maxSum = 150.0f;

		// 距離が大きいほどスケールが小さくなる（滑らかに抑える）
		float scale = maxSum / (sumAbs + maxSum);
		camForward *= scale;
		float Special = 1.0f;
		if (Enemy.GetSpecial()) Special = 1.5f;
		campos = m_player.GetPosition() + camForward *Special;
		campos.y = 30;
		
		/*if (m_camera.GetMovePosition() == Vector3(0.0f, 0.0f, 0.0f)) {
			m_camera.SetPosition(campos);
		}*/
		//カメラに本来のカメラ座標を記録させる
		m_camera.Setcampos(campos);


		//被弾時でないかつ銃を撃ったら反動がある
		if (!m_player.GetInvincibility() && m_player.GetShot()) m_camera.SetVibration(1.0f, 100.0f);
		break;
	}

	//ロックオンの更新
	RockonUpdate();

	//フラグ取得関連
	//特殊攻撃撃ったらクールタイムに入る
	if (m_player.GetBurst()) Specialcool = 0;

	//アップデート後の更新
	m_player.LateUpdate(deltatime);
	//被弾中に画面ノイズ掛ける
	//if (m_player.GetInvincibility()) m_camera.LateUpdate();

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->LateUpdate(deltatime);
	}

	Enemy.LateUpdate(deltatime);

	if (Enemy.GetEnemy()->GetShaderNum() == 2) {
		m_camera.LateUpdate();
	}


	//デバック用のアップデート
	CameraFlip();

	//当たり判定の処理
	Collision_Hit();

	Test += 0.001f;
}

void MecScene::draw(uint64_t deltatime) 
{

	switch (UseCamera) 
	{
	case UseCameraFree: 
		m_cameraF.Draw();
		break;

	case UseCameraNormal:
		m_camera.Draw();
		break;

	case UseCameraRockOn:
		m_camera.Draw();
		break;
	}

	//使用するシェーダーの種類
	int Shadernum = 0;

	for (int i = 0; i < m_objects.size(); i++)
	{
		//シェーダーに優先度決めて設定する
		if (m_objects[i]->GetShaderNum() > Shadernum) 
		{
			Shadernum = m_objects[i]->GetShaderNum();
		}
	}


	if (Enemy.GetEnemy()->GetShaderNum() !=0) Shadernum = Enemy.GetEnemy()->GetShaderNum();

	//被弾中に画面ノイズ掛ける
	if (m_player.GetInvincibility()) Shadernum = 4;

	switch (Shadernum) //シェーダーの番号で使うしぇーだーを決定する
	{
	case 0:
		m_Sceneshader.SetGPU();//通常描画
		//m_Noizeshader.SetGPU();//通常描画

		m_skydome->Draw();	// スカイドームの描画(シェーダー無効化)

		m_field->Draw(); //フィールド描画
		//m_Shadowshader.SetGPU(); //丸影
		break;
	case 1:
		m_Monochromeshader.SetGPU();//モノクロ
		m_skydome->Draw();	// スカイドームの描画(シェーダー無効化)

		m_field->Draw(); //フィールド描画
		break;

	case 2://色収差(フィールド含む)
		//m_Sceneshader.SetGPU();//通常描画

		m_rgbSpritshader.SetGPU();//色収差
		m_skydome->Draw();	// スカイドームの描画(シェーダー無効化)
		m_field->Draw(); //フィールド描画
		break;
	case 3://色収差
		m_Sceneshader.SetGPU();//通常描画

		m_field->Draw(); //フィールド描画
		m_rgbSpritshader.SetGPU();//色収差
		m_skydome->Draw();	// スカイドームの描画(シェーダー無効化)
		break;
	case 4://ノイズ
		m_Noizeshader.SetGPU();//通常描画
		m_skydome->Draw();	// スカイドームの描画(シェーダー無効化)
		m_field->Draw(); //フィールド描画
		break;
	default:
		m_Sceneshader.SetGPU();//例外が入っても通常の描画にしておく
		break;
	}
	
	
	

	//Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(Box_Position);

	// OBB
	std::array<GM31::GE::Collision::BoundingBoxOBB, BOXNUM> obbs;

	// OBB情報をセット
	for (std::size_t i = 0; i < obbs.size(); ++i) {
		obbs[i] = GM31::GE::Collision::SetOBB(
			m_boxSRTs[i].rot,				// 姿勢（回転角度）
			m_boxSRTs[i].pos,				// 中心座標（ワールド）
			m_boxSizes[i].x,				// 幅
			m_boxSizes[i].y,				// 高さ
			m_boxSizes[i].z);				// 奥行
	}

	// BOXの色
	std::array<Color, BOXNUM> colors;

	for (std::size_t i = 0; i < m_boxSRTs.size(); ++i) {
		for (std::size_t j = i + 1; j < m_boxSRTs.size(); ++j) {
			bool sts = GM31::GE::Collision::CollisionOBB(obbs[i], obbs[j]);
			if (sts)
			{
				colors[i] = Color(1, 0, 0, 0.3f);
				colors[j] = Color(1, 0, 0, 0.3f);
			}
			else {
				colors[i] = Color(1, 1, 1, 0.3f);
				colors[j] = Color(1, 1, 1, 0.3f);
			}
		}
	}

	//Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_boxSRTs[0].pos);

	SRT plsrt = m_player.GetSRT();


	plsrt.rot.x += 1.55;
	plsrt.rot.y += 1.55;
	

	//m_player.ModelAABB(minpos, maxpos);

	//Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(plsrt.pos);
	//Matrix4x4 transmtxtes = m_RotationMtx * Matrix4x4::CreateTranslation(m_boxSRTs[1].pos);

	/*m_shapecube->Draw(transmtx, {1.0f,1.0f,1.0f,1.0f});
	m_shapecube2->Draw(transmtxtes, { 1.0f,1.0f,1.0f,1.0f });*/

	/*m_shapecube->Draw(transmtx, colors[0]);
	m_shapecube2->Draw(transmtxtes, colors[1]);*/

	

	m_player.Draw();
	//m_rock.Draw();

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Draw();
	}
	//m_Shadowshader.SetGPU(); //丸影
	Enemy.Draw();

	//透過の関係で一番最後(最終的にはプレイヤーと一番近い敵とでフォワードベクトル取ってその向きに出す)
	if(UseCamera == UseCameraRockOn)
	RockonDraw();

	UIDraw();

}

void::MecScene::dispose() 
{

}

int MecScene::ChangeScene()
{
	if (m_player.GetHP() < 1) //プレイヤーが死んだらリザルト
	{
		m_player.Reset();
		Enemy.Reset();
		Fade_Time = 1000;//フェードかかるように
		m_camera.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		//m_enemys[1]->SetPosition({ 5,7,5 });
		return 4;
	}

	//敵全滅させたらシーン切り替え
	if (Enemy.GetEnemy()->GetHP() > 0)
	{
		
		return 0;//体力残ってるならシーン切り替えない
	}
	//シーンのリセット
	m_player.Reset();
	Enemy.Reset();
	Fade_Time = 1000;//フェードリセット
	m_camera.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	//m_enemys[1]->SetPosition({ 25,7,5 });
	return 2;

	return 0;
	
}

void MecScene::Fade_IN(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;

	Fade_Time -= time_D;
	if (Fade_Time < 0) {
		Fade_Time = 0;
		Fade_Color = 0;
	}
	else  Fade_Color = float(Fade_Time / 1000.0f);

	//materialに適応して読み込む
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, Fade_Color);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;

	m_Screen->SetMaterial(mtrl_Screen);
}

void MecScene::Fade_OUT(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;

	Fade_Time += time_D;
	if (Fade_Time > 1000.0f) Fade_Time = 1000.0f;

	Fade_Color = 1.0f - (Fade_Time / 1000.0f);

	//materialに適応して読み込む
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, Fade_Color);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;

	m_Screen->SetMaterial(mtrl_Screen);
}

void MecScene::PlayerMove()
{
	bool jumpflag = false;

	bool step = false;
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_LSHIFT))//ステップだけ個別で判定しておく
	{
		step = true;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// 左前移動

			float radian;
			radian = PI * 0.75f;

			Object_Speed.x -= sinf(radian) * m_player.GetSpead();
			Object_Speed.z -= cosf(radian) * m_player.GetSpead();

			// 目標角度をセット
			m_Destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 左後移動

			float radian;
			radian = PI * 0.25f;

			Object_Speed.x -= sinf(radian) * m_player.GetSpead();
			Object_Speed.z -= cosf(radian) * m_player.GetSpead();

			// 目標角度をセット
			m_Destrot.y = radian;
		}
		else
		{// 左移動

			float radian;
			radian = PI * 0.50f;

			Object_Speed.x -= sinf(radian) * m_player.GetSpead();
			Object_Speed.z -= cosf(radian) * m_player.GetSpead();

			// 目標角度をセット
			m_Destrot.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// 右前移動

			float radian;
			radian = -PI * 0.75f;

			Object_Speed.x -= sinf(radian) * m_player.GetSpead();
			Object_Speed.z -= cosf(radian) * m_player.GetSpead();

			// 目標角度をセット
			m_Destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 右後移動
			float radian;
			radian = -PI * 0.25f;

			Object_Speed.x -= sinf(radian) * m_player.GetSpead();
			Object_Speed.z -= cosf(radian) * m_player.GetSpead();

			// 目標角度をセット
			m_Destrot.y = radian;
		}
		else
		{// 右移動

			float radian;
			radian = -PI * 0.50f;

			Object_Speed.x -= sinf(radian) * m_player.GetSpead();
			Object_Speed.z -= cosf(radian) * m_player.GetSpead();

			// 目標角度をセット
			m_Destrot.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{// 前移動
		float radian;
		radian = PI;

		Object_Speed.x -= sinf(radian) * m_player.GetSpead();
		Object_Speed.z -= cosf(radian) * m_player.GetSpead();

		// 目標角度をセット
		m_Destrot.y = PI;
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{// 後移動
		float radian;
		radian = 0.0f;

		Object_Speed.x -= sinf(radian) * m_player.GetSpead();
		Object_Speed.z -= cosf(radian) * m_player.GetSpead();

		// 目標角度をセット
		m_Destrot.y = 0.0f;
	}

	

	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE))//ジャンプ
	{
		if (m_player.GetPosition().y < 10)
		{
			jumpflag = true;
		}
	}
	

	Vector3 PL_rotation = m_player.GetRotation();

	// 目標角度と現在角度との差分を求める
	float diffrot = m_Destrot.y - PL_rotation.y;
	if (diffrot > PI)
	{
		diffrot -= PI * 2.0f;
	}
	if (diffrot < -PI)
	{
		diffrot += PI * 2.0f;
	}

	// 比率計算
	PL_rotation.y += diffrot * RATE_ROTATE_PLAYER;
	if (PL_rotation.y > PI)
	{
		PL_rotation.y -= PI * 2.0f;
	}
	if (PL_rotation.y < -PI)
	{
		PL_rotation.y += PI * 2.0f;
	}

	m_player.SetRotation(PL_rotation);

	//ステップしているなら速度を加速
	if (step) 
	{
		Object_Speed.x *= 5;
		//Object_Speed.y *= 5;
		Object_Speed.z *= 5;
	}
	
	//重力の適用
	Object_Speed.y -= GRAVITY;

	if (m_player.GetPosition().y < 9)//元々９ 
	{
		Vector3 p_pos = m_player.GetPosition();
		Object_Speed.y = 0;
		p_pos.y = 9;
		m_player.SetPosition(p_pos);
	}

	if (jumpflag) 
	{
		Object_Speed.y += VALUE_JUMP_PLAYER;
	}
	

	/// 位置移動
	m_player.SetPosition(m_player.GetPosition() + Object_Speed);
	//取り付けられているオブジェクトも同時に動かす

	//この接続地点の取り方多分重いから代用案考える
	//for (int i = 0; i < ADHESIOINGMAX; i++) 
	//{
	//	if (AdhesioingObjects[i]) 
	//	{
	//		if (AdhesioingObjects[i])
	//		{
	//			if (AdhesioingObjects[i]->GetAttribute() == JOINABLE && AdhesioingObjects[i]->GetAdhesioing())
	//			{
	//				AdhesioingObjects[i]->SetPosition(m_player.ConectPos(i) + Object_Speed);//場所
	//				AdhesioingObjects[i]->SetRotation(m_player.GetRotation());//角度
	//			}

	//		}
	//	}
	//}
	
	//Object_Speed = Object_Speed  - rate;
	
	//減速
	Object_Speed.x *= dampingFactor;
	Object_Speed.y *= dampingFactor;
	Object_Speed.z *= dampingFactor;
}

void MecScene::PlayerAdhesion()
{
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_L))//取り付けテスト処理
	{
		//if (AdhesioingObjects[2])return;//既に取り付けてあるなら処理しない

		// 1) プレイヤーの位置を取得
		Vector3 playerPos = m_player.GetPosition();

		// 2) もっとも近いオブジェクト探索
		float minDistSq = std::numeric_limits<float>::max();

		Object* minobj = nullptr;

		for (auto& uptr : m_objects) {
			Object* obj = uptr.get();

			// プレイヤー自身は除外
			if (obj == &m_player) continue;
			// 接続可能オブジェクトじゃない場合も除外
			if (obj->GetAttribute() != JOINABLE) continue;
			//既に接続している場合も除外
			if (obj->GetAdhesioing()) continue;

			Vector3 pos = obj->GetPosition();
			float dx = pos.x - playerPos.x;
			float dy = pos.y - playerPos.y;
			float dz = pos.z - playerPos.z;

			float distSq = dx * dx + dy * dy + dz * dz;
			if (distSq < minDistSq) {
				minDistSq = distSq;
				minobj = obj;
				//旧
				//AdhesioingObjects[2] = obj;
			}

		}

		//旧
		//// 3) 見つかった最も近いオブジェクトにだけ処理を通す
		//if (AdhesioingObjects[2]) {
		//	AdhesioingObjects[2]->SetAdhesioing(true);
		//	AdhesioingObjects[2]->SetPosition(m_player.ConectPos(2));
		//	AdhesioingObjects[2]->Adhesioing();
		//	m_player.TestInt++;
		//}

		//一番近いオブジェクトが取り付け不可じゃないなら
		if (minobj) {
			if (m_player.Canconect(minobj) != 100)
			{
				m_player.Conect(m_player.Canconect(minobj), minobj);
				//minobj->Adhesioing();
			}
		}
		


	}
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_K))//取り外しテスト処理
	{
		//旧
		//if (AdhesioingObjects[2])
		//{
		//	if (AdhesioingObjects[2]->GetAttribute() == JOINABLE)
		//	{
		//		AdhesioingObjects[2]->SetAdhesioing(false);
		//		AdhesioingObjects[2] = nullptr;
		//	}
		//	
		//}

		m_player.ReleaseALL();
	}


}

void MecScene::PlayerShot()
{
	
	// 弾発射
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_J)) {
		
		m_player.Action(Vector3{ 0,0,0 });
		//m_player.SetTarget(m_enemys[0]->GetPosition_P());
		m_player.SetTarget(RockonEnemy->GetPosition_P());
	}

	// 弾発射
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_U)) {
		m_player.SetBurst(true);
	}

	// 敵の射撃フラグ無効化
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_P))
	{
		Enemy.GetEnemy()->ReturnFire();
	}


	//モデル表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_1))
	{
		m_player.DrawModel = true;
		m_player.DrawBone = false;
	}
	//ボーン表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_2))
	{
		m_player.DrawModel = false;
		m_player.DrawBone = true;
	}
	//両方表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_3))
	{
		m_player.DrawModel = true;
		m_player.DrawBone = true;
	}
	
}

void MecScene::AddSpeed(float initSpeed, Vector3 Speed)
{
	// 新しい速度を計算
	float newSpeedX = Speed.x * initSpeed;
	float newSpeedY = Speed.y * initSpeed;
	float newSpeedZ = Speed.z * initSpeed;

	// 速度更新時に絶対値で判定し、方向を維持する
	if (fabs(Object_Position.x) < fabs(newSpeedX)) Object_Position.x = newSpeedX;
	if (fabs(Object_Position.y) < fabs(newSpeedY)) Object_Position.y = newSpeedY;
	if (fabs(Object_Position.z) < fabs(newSpeedZ)) Object_Position.z = newSpeedZ;

}

void MecScene::SetSpeed(Vector3 Speed)
{
	Object_Position = Speed;
}

void MecScene::Collision_Hit()//弾とオブジェクトの当たり判定

	//	ここめちゃくちゃ重いから軽量化の手段考えておく
{

	if (Enemy.GetEnemy()->GetHP() <= 0) //敵が既に死んでいるなら次
	{
		return;
	}

	for (int i = 0; i < Enemy.GetEnemy()->GetBulletMaxnum(); i++)//この敵の弾がプレイヤーにあたっているか
	{
		/*bool col = GM31::GE::Collision::CollisionOBB(m_player.GetOBB(), m_enemys[0]->GetOBB_Bullet(i));*/

		if (m_player.GetInvincibility()) continue; //プレイヤーの無敵時間中なら判定しない
		if (Enemy.GetEnemy()->GetBulletcol(i))continue;//検査する弾丸が使われていないなら検査しない

		bool col = m_player.Collision_PL(Enemy.GetEnemy()->GetOBB_Bullet(i));
		m_player.SetCol(col);

		if (col)
		{
			m_player.HitDamage(Enemy.GetEnemy()->Damage_Bullet());
			Enemy.GetEnemy()->SetCollision_Bullet(i, col);
			//カメラ揺れを入れる
			m_camera.SetVibration(10.0f, 1000);
		}
	}

	if (m_player.Collision_PL(Enemy.GetEnemy()->GetOBB_Beam())) //この敵のビームがプレイヤーにあたっているか
	{
		m_player.SetCol(true);
		m_player.HitDamage(Enemy.GetEnemy()->Damage_Beam());
	}

	//for (int i = 0; i < ADHESIOINGMAX; i++) //銃を親オブジェクトとした弾と敵の当たり判定
	//{
	//	if (!AdhesioingObjects[i]) continue;//ネスト長くなるからここはifの中身じゃなくてcontinueで返す
	//	if (auto gun = dynamic_cast<M_Gun*>(AdhesioingObjects[i]))
	//	{
	//		for (int i = 0; i < 5; i++)
	//		{
	//			bool col = GM31::GE::Collision::CollisionOBB(Enemy.GetEnemy()->GetOBB(), gun->GetOBB_Bullet(i));
	//			bool inter = GM31::GE::Collision::CollisionSphereOBB_(Enemy.GetEnemy()->GetShere(), gun->GetOBB_Bullet(i));
	//			Enemy.GetEnemy()->SetCollision(col);
	//			if (col) gun->SetCollision_Bullet(i, col);
	//			if (inter) {
	//				Enemy.GetEnemy()->SetAvoidance(inter);
	//				Enemy.GetEnemy()->Stepavoidance(gun->GetPosition());
	//				//gun->SetCollision_Bullet(i, inter);
	//			}
	//		}
	//	}
	//}
	
}

void MecScene::RockonUpdate()
{
	if (!RockonEnemy)return;


	//ロックオン対象のHPがなくなるかQキーで対象を更新する
	if (RockonEnemy->GetHP() == 0 || CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q))
	{
		// 1) プレイヤーの位置を取得
		Vector3 playerPos = m_player.GetPosition();

		// 2) もっとも近いオブジェクト探索
		float minDistSq = std::numeric_limits<float>::max();

		Vector3 pos = Enemy.GetEnemy()->GetPosition();
		float dx = pos.x - playerPos.x;
		float dy = pos.y - playerPos.y;
		float dz = pos.z - playerPos.z;

		float distSq = dx * dx + dy * dy + dz * dz;
		if (distSq < minDistSq) {
			minDistSq = distSq;
			//RockonEnemy = Enemy.GetEnemy();
			m_player.SetTarget(Enemy.GetEnemy()->GetPosition_P());
		}
	}
	
}

void MecScene::RockonDraw()
{
	//座標更新前に角度を更新する
	// 事前に #include <cmath> などが必要
	const float eps = 1e-6f;

	// 1. forward ベクトルを計算
	//Vector3 camForward = (m_player.GetPosition() - m_enemys[0]->GetPosition());
	//Vector3 enemypos = RockonEnemy->GetCenter();
	Vector3 enemypos = RockonEnemy->GetPosition();
	//enemypos.y + 30;
	//enemypos.x + 3;
	Vector3 camForward = (m_player.GetPosition() - enemypos);

	// 2. 動きがある場合のみ回転計算
	if (camForward.LengthSquared() > eps) {
		
	}

	camForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(camForward.x, camForward.z);
	float pitch = atan2f(-camForward.y,
		sqrtf(camForward.x * camForward.x + camForward.z * camForward.z));

	// 4. Roll は今回は固定 0
	camRot = Vector3{ pitch, yaw, 0.0f };

	//m_image->Draw(Vector3{ 1,1,1 }, camRot, m_enemys[0]->GetPosition() + (camForward * 10.0));
	m_Rockon->Draw3D(Vector3{ 1,1,1 }, camRot, RockonEnemy->GetPosition() + (camForward * 10.0));
}

void MecScene::UIDraw()
{
	//座標更新前に角度を更新する
	// 事前に #include <cmath> などが必要
// UIの中央位置（スクリーン座標からワールド座標へ）
	Vector3 centerPos = m_camera.ScreenToWorld(850, 100, 0.5);//Test,200,0.5
	//Vector3 centerPos = m_camera.ScreenToWorld(0, 0, 0.5);//Test,200,0.5

	// カメラからUIへの方向ベクトル
	Vector3 camForward = (m_camera.GetPosition() - centerPos);
	camForward.Normalize();

	// 回転計算
	float yaw = atan2f(camForward.x, camForward.z);
	float pitch = atan2f(-camForward.y,
		sqrtf(camForward.x * camForward.x + camForward.z * camForward.z));
	Vector3 camRotUI = { pitch, yaw, 0.0f };

	//敵のHP表示
	float imageScalex = 0.04f / RockonEnemy->GetMaxHP();
	float currentWidth = imageScalex * RockonEnemy->GetHP();
	float currentWidth_First = imageScalex * RockonEnemy->GetMaxHP();

	// カメラの右方向ベクトル（ワールド空間）
	Vector3 rightVec = Vector3::TransformNormal(
		Vector3::Right,
		Matrix4x4::CreateFromYawPitchRoll(camRotUI.y, camRotUI.x, camRotUI.z)
	);
	rightVec.Normalize();

	// 左端固定にするため、中央位置から左に半分戻す
	Vector3 leftBasePos = centerPos - rightVec * (0.04f * 0.5f); // 0.04fは最大幅

	// 現在の幅に応じて右に伸ばす（左端は固定）
	Vector3 adjustedPos = leftBasePos + rightVec * (currentWidth * 0.5f);

	// 左端固定のためのオフセット
	//Vector3 offset = rightVec * (currentWidth * 0.5f);
	Vector3 offset = rightVec * (HP_Enemy_G->GetWidth() * currentWidth * 0.5f);
	Vector3 offset_First = rightVec * (HP_Enemy_G->GetWidth() * currentWidth_First * 0.5f);
	offset_First += camForward * 0.01f;

	//ここでカメラのビルボード計算する
	// バーの中心位置からカメラへの方向ベクトル
	Vector3 camRange_HP_R = (m_camera.GetPosition() - (leftBasePos + offset_First));
	camRange_HP_R.Normalize();

	// 回転計算
	yaw = atan2f(camRange_HP_R.x, camRange_HP_R.z);
	pitch = atan2f(-camRange_HP_R.y,
		sqrtf(camRange_HP_R.x * camRange_HP_R.x + camRange_HP_R.z * camRange_HP_R.z));

	Vector3 camRot_HP_R = { pitch, yaw, 0.0f };


	//カメラへの向き取ってビルボード作る
	Vector3 camRange_HP_G = (m_camera.GetPosition() - (leftBasePos + offset));
	camRange_HP_G.Normalize();

	// 回転計算
	yaw = atan2f(camRange_HP_G.x, camRange_HP_G.z);
	pitch = atan2f(-camRange_HP_G.y,
		sqrtf(camRange_HP_G.x * camRange_HP_G.x + camRange_HP_G.z * camRange_HP_G.z));

	Vector3 camRot_HP_G = { pitch, yaw, 0.0f };

	//座標補正
	//Vector3 halfOffset_R = rightVec * (currentWidth_First * 0.5f);
	Vector3 halfOffset_R = rightVec * (currentWidth_First * 0.5f) - camRange_HP_R * 0.1;
	Vector3 halfOffset_G = rightVec * (currentWidth * 0.5f);

	// 左端を worldLeftPos とする
	Vector3 worldLeftPos = leftBasePos;

	// 左端位置からオフセットを加えて描画
	//camRotUIよりcamRotの方が精度高いので修正
	//HP_Enemy_R->Draw3D(Vector3{ currentWidth_First, 0.007f, 0.03f }, camRot_HP_R, worldLeftPos + halfOffset_R - offset_First);
	//HP_Enemy_G->Draw3D(Vector3{ currentWidth, 0.007f, 0.03f }, camRot_HP_G, worldLeftPos + halfOffset_G - offset);
	HP_Enemy_R->Draw(Vector3{ 1.7f, 0.25f, 1.0f }, Vector3(0.0f, 0.0f, 0.0f), Vector3(1000.0f, 100.0f, 0.1f));
	HP_Enemy_G->Draw(Vector3{ 1.7f * float(RockonEnemy->GetHP()) / float(RockonEnemy->GetMaxHP()), 0.25f, 1.0f }, Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1180.0f - 180.0f * float(RockonEnemy->GetHP()) / float(RockonEnemy->GetMaxHP()), 100.0f, 0.0f));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//プレイヤーのHP表示
	centerPos = m_camera.ScreenToWorld(20, 100, 0.5);//HPバーの表示位置
	imageScalex = 0.04f / m_player.GetMaxHP();
	currentWidth = imageScalex * m_player.GetHP();
	currentWidth_First = imageScalex * m_player.GetMaxHP();

	// 左端固定にするため、中央位置から左に半分戻す
	leftBasePos = centerPos - rightVec * (0.04f * 0.5f); // 0.04fは最大幅

	// 現在の幅に応じて右に伸ばす（左端は固定）
	adjustedPos = leftBasePos + rightVec * (currentWidth * 0.5f);

	// 左端固定のためのオフセット
	//Vector3 offset = rightVec * (currentWidth * 0.5f);
	offset = rightVec * (HP_Player_G->GetWidth() * currentWidth * 0.5f);
	offset_First = rightVec * (HP_Player_G->GetWidth() * currentWidth_First * 0.5f);
	offset_First += camForward * 0.01f;

	// 左端位置からオフセットを加えて描画
		//camRotUIよりcamRotの方が精度高いので修正
	HP_Player_R-> Draw(Vector3{ 1.7f, 0.25f, 1.0f }, Vector3(0.0f, 0.0f, 0.0f), Vector3(300.0f, 100.0f, 0.1f));
	//HP_Player_G->Draw(Vector3{ 1.7f, 0.25f, 1.0f }, Vector3(0.0f, 0.0f, 0.0f), Vector3(300.0f, 100.0f, 0.0f));
	HP_Player_G->Draw(Vector3{ 1.7f * float(m_player.GetHP())/float(m_player.GetMaxHP()), 0.25f, 1.0f}, Vector3(0.0f, 0.0f, 0.0f), 
		Vector3(120.0f  + 180.0f* float(m_player.GetHP()) / float(m_player.GetMaxHP()), 100.0f, 0.0f));

	m_Special->Draw(Vector3{ 1.7f * (Specialcool / 5000), 0.17f, 1.0f }, Vector3(0.0f, 0.0f, 0.0f),
		Vector3(120.0f + 180.0f * Specialcool / 5000, 150.0f, 0.0f));

	m_Tutorial->Draw(Vector3{ 3,1,1 }, { 0,0,0 }, { 300,670,0 });

	//HP_Player_R->Draw(Vector3(1, 1, 1), Vector3(0, 0, 0), Vector3(100, 100, 0));
	//画面青色のエフェクト掛けてコクピットっぽい写りにしたい
	m_Screen->Draw(Vector3{ 7,5,1}, Vector3(0, 0, 0), Vector3(650, 340, 0));
}

void MecScene::CameraFlip()
{
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_0))
	{
		int idx = UseCamera;
		++idx;
		//if (idx < 0) idx = CAMERA_MAX - 1;
		if (idx >= CAMERA_MAX) idx = 0;
		UseCamera = idx;
		//ロックオンしたときにロックオン対象を一番近い敵に更新
		if (UseCamera == UseCameraRockOn) 
		{
			// 1) プレイヤーの位置を取得
			Vector3 playerPos = m_player.GetPosition();

			// 2) もっとも近いオブジェクト探索
			float minDistSq = std::numeric_limits<float>::max();

			Vector3 pos = Enemy.GetEnemy()->GetPosition();
			float dx = pos.x - playerPos.x;
			float dy = pos.y - playerPos.y;
			float dz = pos.z - playerPos.z;

			float distSq = dx * dx + dy * dy + dz * dz;
			if (distSq < minDistSq) {
				minDistSq = distSq;
				RockonEnemy = Enemy.GetEnemy();
				m_player.SetTarget(Enemy.GetEnemy()->GetPosition_P());
			}
		}
	}
	
}

void MecScene::Debug_Box()
{
	ImGui::Begin("debug Box SRT");

	for (uint8_t idx = 0; idx < m_boxSRTs.size(); idx++) {
		std::string str;
		str = "Box" + std::to_string(idx);

		ImGui::Text(str.c_str());
		ImGui::SliderFloat3((str + std::string(" pos")).c_str(), &m_boxSRTs[idx].pos.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" rot")).c_str(), &m_boxSRTs[idx].rot.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" size")).c_str(), &m_boxSizes[idx].x, 1, 100);
	}

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

// デバッグフリーカメラ
void MecScene::debugFreeCamera()
{
	ImGui::Begin("debug Free camera");

	static float radius = 100.0f;
	static Vector3 pos = Vector3(0, 0, radius);
	static Vector3 lookat = Vector3(0, 0, 0);
	static float elevation = -90.0f * PI / 180.0f;
	static float azimuth = PI / 2.0f;

	static Vector3 spherecenter = Vector3(0, 0, 0);

	ImGui::SliderFloat("Radius", &radius, 1, 800);
	ImGui::SliderFloat("Elevation", &elevation, -PI, PI);
	ImGui::SliderFloat("Azimuth", &azimuth, -PI, PI);

	ImGui::SliderFloat3("lookat ", &lookat.x, -100, 100);

	//// カメラの位置を極座標からデカルト座標に変換
	m_cameraF.SetRadius(radius);
	m_cameraF.SetElevation(elevation);
	m_cameraF.SetAzimuth(azimuth);
	m_cameraF.SetLookat(lookat);

	// カメラの位置を極座標から求める
	m_cameraF.CalcCameraPosition();

	ImGui::End();
}

void MecScene::PlayerMovetes()
{
	bool step = CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_LSHIFT);
	bool jumpf = false;

	// ジャンプ判定
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE)
		&& m_player.GetPosition().y < 10.0f)
	{
		jumpf = true;
	}

	// 左右・前後の入力をローカル座標系で集約
	Vector3 localDir = { 0, 0, 0 };
	auto& DI = CDirectInput::GetInstance();

	if (DI.CheckKeyBuffer(DIK_W)) { localDir += { 0, 0, 1 };
	Vector3 camposition = campos + m_player.GetForward() * 20;
	m_camera.SetMovePosition(camposition);
	}  // 前
	if (DI.CheckKeyBuffer(DIK_S)) {
		localDir += { 0, 0, -1 };
		Vector3 camposition = campos - m_player.GetForward() * 20;
		m_camera.SetMovePosition(camposition);
	}  // 後
	if (DI.CheckKeyBuffer(DIK_A)) { 
		localDir += {-1, 0, 0 };
		Vector3 camposition = campos + m_player.GetRight() * 20;
		m_camera.SetMovePosition(camposition);
	}  // 左
	if (DI.CheckKeyBuffer(DIK_D)) { 
		localDir += { 1, 0, 0 }; 
		Vector3 camposition = campos - m_player.GetRight() * 20;
		m_camera.SetMovePosition(camposition);
	}// 右

	if (localDir.LengthSquared() > 1e-6f) localDir.Normalize();  // 斜め移動時も速度一定

	//カメラのがたがた補正する
	if (localDir.LengthSquared() > 0)
	{
		localDir.Normalize();
		m_camera.SetMovePosition(campos + localDir * 20.0f);
	}
	else
	{
		m_camera.SetMovePosition(campos);
	}

	

	if (localDir.LengthSquared() > 1e-6f)
	{
		localDir.Normalize();

		// 2. ワールド変換（）
		Vector3 worldDir = Vector3{ 0,0,0 };
		if (UseCamera == UseCameraNormal) 
		{
			////プレイヤーの向き基準
			//float yaw = m_player.GetRotation().y;
			//Matrix4x4 rot = Matrix4x4::CreateRotationY(yaw);
			//worldDir = Vector3::Transform(localDir, rot);
			//カメラの向き基準
			float camYaw = 0;
			auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
			worldDir = Vector3::Transform(localDir, rotMatCam);
		}
		else if (UseCamera == UseCameraRockOn) 
		{
			//カメラの向き基準
			float camYaw = camRot.y;
			auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
			worldDir = Vector3::Transform(localDir, rotMatCam);
		}
		else //それ以外ならとりあえずカメラに合わせる
		{
			//カメラの向き基準
			float camYaw = 0;
			auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
			worldDir = Vector3::Transform(localDir, rotMatCam);
		}

		//カメラの向き基準
		float camYaw = camRot.y;
		auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
		worldDir = Vector3::Transform(localDir, rotMatCam);
		
		// 移動量に乗じる前に反転
		worldDir = -worldDir;

		// 3. ステップ時は加速度UP
		float speedScale = m_player.GetSpead() * (step ? 5.0f : 1.0f);
		Object_Speed += worldDir * speedScale;

		// 4. 向き目標を設定（ワールド方向ベクトルから計算）
		m_Destrot.y = atan2f(-worldDir.x, -worldDir.z);
	}

	// 5. 重力・ジャンプ・摩擦 etc.
	Object_Speed.y -= GRAVITY;
	if (m_player.GetPosition().y < 9.0f)
	{
		auto p = m_player.GetPosition();
		p.y = 9.0f;  Object_Speed.y = 0;
		m_player.SetPosition(p);
	}
	if (jumpf) Object_Speed.y += VALUE_JUMP_PLAYER;

	// 6. 回転補間
	Vector3 rot = m_player.GetRotation();
	float diff = m_Destrot.y - rot.y;
	if (diff > PI) diff -= 2 * PI;
	if (diff < -PI) diff += 2 * PI;
	rot.y += diff * RATE_ROTATE_PLAYER;
	if (rot.y > PI) rot.y -= 2 * PI;
	if (rot.y < -PI) rot.y += 2 * PI;
	m_player.SetRotation(rot);

	//ステップ
	if (step) {
		Object_Speed.x *= 5;
		Object_Speed.z *= 5;
		if (Object_Speed.y < 0) //若干急降下気味に降りる方が感触良かったから残す
		{
			Object_Speed.y *= 2.5;
		}
	}

	// 7. 位置更新＋減速
	m_player.SetPosition(m_player.GetPosition() + Object_Speed);

	//この接続地点の取り方多分重いから代用案考える
	//for (int i = 0; i < ADHESIOINGMAX; i++) 
	//{
	//	if (AdhesioingObjects[i]) 
	//	{
	//		if (AdhesioingObjects[i])
	//		{
	//			if (AdhesioingObjects[i]->GetAttribute() == JOINABLE && AdhesioingObjects[i]->GetAdhesioing())
	//			{
	//				AdhesioingObjects[i]->SetPosition(m_player.ConectPos(i) + Object_Speed);//場所
	//				AdhesioingObjects[i]->SetRotation(camRot);//角度
	//			}

	//		}
	//	}
	//}
	
	Object_Speed *= dampingFactor;

}
