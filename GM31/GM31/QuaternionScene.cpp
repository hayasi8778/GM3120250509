#include "QuaternionScene.h"

QuaternionScene::QuaternionScene()
{

}

void QuaternionScene::update(uint64_t deltatime)
{
	angle += 0.1;//回転

	//ここでベクトルをクォータニオンに変換して掛け算を行う
	
	Vector3 axis = { 0.0f,0.0f,0.0f };

	// クオータニオン生成
	Quat.x = axis.x * sin(angle / 2.0f);
	Quat.y = axis.y * sin(angle / 2.0f);
	Quat.z = axis.z * sin(angle / 2.0f);
	Quat.w = cos(angle / 2.0f);

	//ベクトル(針の長さ?)
	
	Vector3 e = { 300,200,0 };
	Vector3 p1;

	p1.x = e.x - s.x;
	p1.y = e.y - s.y;
	p1.z = e.z - s.z;

	// ３次元ベクトルをクオータニオンにする
	vectorQuat.x = p1.x;
	vectorQuat.y = p1.y;
	vectorQuat.z = p1.z;
	vectorQuat.w = 0.0f;

	//共役クォータニオン()
	Quaternion ConjugationQuat;

	ConjugationQuat.x = -Quat.x;				// 共役クオータニオンを作る
	ConjugationQuat.y = -Quat.y;
	ConjugationQuat.z = -Quat.z;
	ConjugationQuat.w = Quat.w;

	//時計回り
	AnswerQuat = Quaternion::Concatenate(Quat, vectorQuat);		// 掛け算
	AnswerQuat = Quaternion::Concatenate(AnswerQuat, ConjugationQuat);	// 掛け算

	
}

void QuaternionScene::draw(uint64_t deltatime)
{
	//MoveToEx(始点)とLineTo(終点)で描画っぽいけどウィンドウハンドルいるみたいだから受け渡し必須?

	//Draw関数は多分四角形しか出力出来なそうだったからDrawLine使う
	/*
	ID3D11DeviceContext* deviceContext;
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// シェーダーを適用し、描画
	deviceContext->Draw(2, 0);
	*/


}

void QuaternionScene::init()
{
}

void QuaternionScene::dispose()
{
}
