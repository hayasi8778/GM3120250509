#pragma once
#include "system/commontypes.h"
#include "system/CShader.h"
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include	"system/collision.h"

#include "Range.h"

//ほぼ全部が参照する基底クラスになるからdot処理ここに入れる
// inline/constexpr にしてコンパイラ最適化を効かせる
inline float Dot(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}



//ゲームに出すオブジェクト用の親クラス

//--- 定数定義
enum ObjectAttribute //オブジェクトの役割
{
	DEFAULT_ATTRIBUTE, //定義していない場合は基本動かしたくない
	PLAYER, 
	ENEMY,
	JOINABLE, //接合可能なオブジェクト
	UNJOINABLE //接合不可なオブジェクト
};

//オブジェクトの識別用の
enum ObjectType 
{
	DEFAULT_TYPE,	//未定義
	CHARACTER,	//キャラクター(基本接続不可)
	GUN,		//銃(このオブジェクトから弾を打つ)
	PROJECTILE	//投射物(このオブジェクト自体を投げつけて攻撃する)
};

class Object {
protected:
	//オブジェクトの種類
	int Attribute = DEFAULT_ATTRIBUTE;
	int MyType = DEFAULT_TYPE;
	//どの部位に接続可能なオブジェクトか 
	//						頭   胴体    左腕   右腕   左足   右足
	bool Connectable[6] = { false,false ,false ,false ,false ,false };

	// SRT情報（姿勢情報）
	Vector3	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3	m_Scale = Vector3(1.0f, 1.0f, 1.0f);

	//プレイヤーと接着しているか
	bool adhesioing = false;

	// 描画の為の情報（メッシュに関わる情報）
	CStaticMeshRenderer	m_meshrenderer;
	CStaticMesh			m_mesh;							// メッシュデータ

	// 移動量
	Vector3	m_Move = { 0.0f,0.0f,0.0f };

	// 目標回転角度
	Vector3	m_Destrot = { 0.0f,0.0f,0.0f };

	// 描画の為の情報（見た目に関わる部分）
	//CShader			m_shader;	// シェーダ
public:
	Object() = default;
	virtual ~Object() = default;
	virtual void Init() = 0;
	virtual void Update(uint64_t deltatime) = 0;
	virtual void LateUpdate(uint64_t deltatime) = 0;//Update→Actionの順で動くので最終的にフラグを切るのはここで行う
	virtual void Draw() = 0;
	virtual void Dispose() = 0;
	virtual void Adhesioing() = 0;
	virtual void Action(Vector3 vec) = 0;
	//virtual void Reset() = 0;
	virtual int GetShaderNum() = 0;
	virtual GM31::GE::Collision::BoundingBoxOBB GetOBB() = 0;
	
	bool  CanConnectable(int i) { return  Connectable[i]; }

	int GetAttribute() { return Attribute; }//ゲッターだけ

	void SetPosition(Vector3 pos) { m_Position = pos; }
	void SetRotation(Vector3 rot) { m_Rotation = rot; }
	void SetScale(Vector3 scale) { m_Scale = scale; }
	void SetAdhesioing(bool Adh) { adhesioing = Adh; }

	Vector3 GetPosition()  const { return m_Position; }
	Vector3* GetPosition_P() { return &m_Position; }
	Vector3 GetRotation() const { return m_Rotation; }
	Vector3 GetScale()  const { return m_Scale; }
	bool GetAdhesioing() { return adhesioing; }

	SRT GetSRT() const {
		SRT srt;
		srt.pos = m_Position;
		srt.rot = m_Rotation;
		srt.scale = m_Scale;
		return srt;
	}

	const CStaticMesh& GetMesh() {
		return m_mesh;
	}

	//モデルのmaxminを返す関数を作っておく
	void ModelAABB(aiVector3D& outMin, aiVector3D& outMax) { m_meshrenderer.ComputeModelAABB(outMin, outMax); }

};

