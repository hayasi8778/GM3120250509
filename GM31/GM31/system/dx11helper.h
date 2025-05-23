#pragma once
#include	<d3dcompiler.h>
#include	<locale.h>
#include	<d3d11.h>
#include	<memory>
#include	<string>

HRESULT CompileShader(const char* szFileName, 
	LPCSTR szEntryPoint, 
	LPCSTR szShaderModel, 
	void** ShaderObject, 
	size_t& ShaderObjectSize, 
	ID3DBlob** ppBlobOut);

HRESULT CompileShaderFromFile(
	const char* szFileName, 
	LPCSTR szEntryPoint, 
	LPCSTR szShaderModel, 
	ID3DBlob** ppBlobOut);

bool CreateConstantBuffer(
	ID3D11Device* device,
	unsigned int bytesize,
	ID3D11Buffer** pConstantBuffer			// コンスタントバッファ
	);

bool CreateConstantBufferWrite(
	ID3D11Device* device,					// デバイスオブジェクト
	unsigned int bytesize,					// コンスタントバッファサイズ
	ID3D11Buffer** pConstantBuffer			// コンスタントバッファ
);

bool CreateIndexBuffer(
	ID3D11Device* device,
	unsigned int indexnum,					// 頂点数
	void* indexdata,						// インデックスデータ格納メモリ先頭アドレス
	ID3D11Buffer** pIndexBuffer);

bool CreateVertexBuffer(
	ID3D11Device* device,
	unsigned int stride,				// １頂点当たりバイト数
	unsigned int vertexnum,				// 頂点数
	void* vertexdata,					// 頂点データ格納メモリ先頭アドレス
	ID3D11Buffer** pVertexBuffer		// 頂点バッファ
	);

bool CreateVertexBufferWrite(
	ID3D11Device* device,
	unsigned int stride,				// １頂点当たりバイト数
	unsigned int vertexnum,				// 頂点数
	void* vertexdata,					// 頂点データ格納メモリ先頭アドレス
	ID3D11Buffer** pVertexBuffer		// 頂点バッファ
);

bool CreateVertexBufferUAV(
	ID3D11Device* device,
	unsigned int stride,				// １頂点当たりバイト数
	unsigned int vertexnum,				// 頂点数
	void* vertexdata,					// 頂点データ格納メモリ先頭アドレス
	ID3D11Buffer** pVertexBuffer		// 頂点バッファ
	);

bool CreateStructuredBuffer(
	ID3D11Device* device,
	unsigned int stride,				// ストライドバイト数
	unsigned int num,					// 個数
	void* data,							// データ格納メモリ先頭アドレス
	ID3D11Buffer** pStructuredBuffer	// StructuredBuffer
	);

ID3D11Buffer* CreateAndCopyToBuffer(
	ID3D11Device* device,
	ID3D11DeviceContext* devicecontext,
	ID3D11Buffer* pBuffer	// RWStructuredBuffer
	);

bool CreateShaderResourceView(			// バッファからシェーダーリソースビューを作成する	
	ID3D11Device* device,
	ID3D11Buffer* pBuffer,				// Buffer
	ID3D11ShaderResourceView** ppSRV);

bool CreateUnOrderAccessView(
	ID3D11Device* device,
	ID3D11Buffer* pBuffer,				// Buffer
	ID3D11UnorderedAccessView** ppUAV);

bool CreateVertexShader(ID3D11Device* device,		// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
	const char* szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	D3D11_INPUT_ELEMENT_DESC* layout,
	unsigned int numElements,
	ID3D11VertexShader** ppVertexShader,
	ID3D11InputLayout**  ppVertexLayout);


bool CreatePixelShader(ID3D11Device* device,		// ピクセルシェーダーオブジェクトを生成
	const char* szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11PixelShader** ppPixelShader);

bool CreateGeometryShader(ID3D11Device* device,
	const char* szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11GeometryShader** ppGeometryShader);		// ジオメトリシェーダーオブジェクトを生成

std::string ExtractFileName(std::string fullpath, char split);