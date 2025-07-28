#pragma once
#include "Object.h"
#include "Buller.h"

#include "system/DebugUI.h"

class M_Player : public Object 
{
private:
	Bullet m_bullet;//プレイヤーが発射する弾

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Dispose() override;
	void Debug_Player();//デバック用GUI一式
	Vector3 ConectPos();
		
};