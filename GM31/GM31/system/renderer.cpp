/**
 * @file Renderer.cpp
 * @brief DirectX ��p���������_�����O�������������� Renderer �N���X�̒�`�i�������j�B
 *
 * ���̃t�@�C���ł́ADirect3D �f�o�C�X�A�X���b�v�`�F�[���A�����_�[�^�[�Q�b�g�A�萔�o�b�t�@�Ȃǂ�
 * �������Ɖ���A�`��̊J�n�E�I�������A�e�탌���_�����O��ԁi�[�x�A�u�����h�A�}�g���b�N�X�Ȃǁj�̐ݒ���s���܂��B
 */

#include <stdexcept>
#include "renderer.h"
#include "../Application.h"

//------------------------------------------------------------------------------
// �X�^�e�B�b�N�����o�ϐ��̏�����
//------------------------------------------------------------------------------

D3D_FEATURE_LEVEL       Renderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ComPtr<ID3D11Device> Renderer::m_Device;
ComPtr<ID3D11DeviceContext> Renderer::m_DeviceContext;
ComPtr<IDXGISwapChain> Renderer::m_SwapChain;
ComPtr<ID3D11RenderTargetView> Renderer::m_RenderTargetView;
ComPtr<ID3D11DepthStencilView> Renderer::m_DepthStencilView;

ComPtr<ID3D11Buffer> Renderer::m_WorldBuffer;
ComPtr<ID3D11Buffer> Renderer::m_ViewBuffer;
ComPtr<ID3D11Buffer> Renderer::m_ProjectionBuffer;
ComPtr<ID3D11Buffer> Renderer::m_MaterialBuffer;
ComPtr<ID3D11Buffer> Renderer::m_LightBuffer;

ComPtr<ID3D11DepthStencilState> Renderer::m_DepthStateEnable;
ComPtr<ID3D11DepthStencilState> Renderer::m_DepthStateDisable;

ComPtr<ID3D11BlendState> Renderer::m_BlendState[MAX_BLENDSTATE];
ComPtr<ID3D11BlendState> Renderer::m_BlendStateATC;

//------------------------------------------------------------------------------
// Renderer �N���X�̊e�֐��̎���
//------------------------------------------------------------------------------

/**
 * @brief Renderer �̏������������s���܂��B
 *
 * Direct3D �f�o�C�X�ƃX���b�v�`�F�[���̍쐬�A�����_�[�^�[�Q�b�g�r���[�A�f�v�X�X�e���V���r���[�A
 * �r���[�|�[�g�A���X�^���C�U�A�u�����h�X�e�[�g�A�[�x�X�e���V���X�e�[�g�A�T���v���[�X�e�[�g�A
 * �萔�o�b�t�@�̐����A�������C�g����у}�e���A���̐ݒ�Ȃǂ����{���܂��B
 */
void Renderer::Init()
{
    HRESULT hr = S_OK;

    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = Application::GetWidth();
    swapChainDesc.BufferDesc.Height = Application::GetHeight();
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = Application::GetWindow();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
        m_SwapChain.GetAddressOf(),
        m_Device.GetAddressOf(),
        &m_FeatureLevel,
        m_DeviceContext.GetAddressOf());

    ComPtr<ID3D11Texture2D> renderTarget;
    hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(renderTarget.GetAddressOf()));
    if (SUCCEEDED(hr) && renderTarget) {
        m_Device->CreateRenderTargetView(renderTarget.Get(), nullptr, m_RenderTargetView.GetAddressOf());
    }
    else {
        throw std::runtime_error("Failed to retrieve render target buffer.");
    }

    ComPtr<ID3D11Texture2D> depthStencil;
    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = swapChainDesc.BufferDesc.Width;
    textureDesc.Height = swapChainDesc.BufferDesc.Height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_D16_UNORM;
    textureDesc.SampleDesc = swapChainDesc.SampleDesc;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = m_Device->CreateTexture2D(&textureDesc, nullptr, depthStencil.GetAddressOf());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create depthStencil.");
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = textureDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    hr = m_Device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_DepthStencilView.GetAddressOf());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create depthStencilView.");
    }

    m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(Application::GetWidth());
    viewport.Height = static_cast<FLOAT>(Application::GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_DeviceContext->RSSetViewports(1, &viewport);


    // --- ���X�^���C�U�X�e�[�g�ݒ� ---
    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthClipEnable = TRUE;

    ComPtr<ID3D11RasterizerState> rs;
    m_Device->CreateRasterizerState(&rasterizerDesc, rs.GetAddressOf());
    m_DeviceContext->RSSetState(rs.Get());

    // --- �u�����h�X�e�[�g�̐��� ---
    D3D11_BLEND_DESC BlendDesc{};
    BlendDesc.AlphaToCoverageEnable = FALSE;
    BlendDesc.IndependentBlendEnable = TRUE;
    BlendDesc.RenderTarget[0].BlendEnable = FALSE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_Device->CreateBlendState(&BlendDesc, m_BlendState[0].GetAddressOf());
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    m_Device->CreateBlendState(&BlendDesc, m_BlendState[1].GetAddressOf());
    m_Device->CreateBlendState(&BlendDesc, m_BlendStateATC.GetAddressOf());

    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    m_Device->CreateBlendState(&BlendDesc, m_BlendState[2].GetAddressOf());

    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
    m_Device->CreateBlendState(&BlendDesc, m_BlendState[3].GetAddressOf());

    SetBlendState(BS_ALPHABLEND);

    // --- �[�x�X�e���V���X�e�[�g�̐ݒ� ---
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;

    m_Device->CreateDepthStencilState(&depthStencilDesc, m_DepthStateEnable.GetAddressOf());

    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    m_Device->CreateDepthStencilState(&depthStencilDesc, m_DepthStateDisable.GetAddressOf());

    m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable.Get(), 0);

    // --- �T���v���[�X�e�[�g�ݒ� ---
    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = 4;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ComPtr<ID3D11SamplerState> samplerState;
    m_Device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
    m_DeviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    // --- �萔�o�b�t�@���� ---
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(Matrix4x4);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(float);

    m_Device->CreateBuffer(&bufferDesc, nullptr, m_WorldBuffer.GetAddressOf());
    m_DeviceContext->VSSetConstantBuffers(0, 1, m_WorldBuffer.GetAddressOf());

    m_Device->CreateBuffer(&bufferDesc, nullptr, m_ViewBuffer.GetAddressOf());
    m_DeviceContext->VSSetConstantBuffers(1, 1, m_ViewBuffer.GetAddressOf());

    m_Device->CreateBuffer(&bufferDesc, nullptr, m_ProjectionBuffer.GetAddressOf());
    m_DeviceContext->VSSetConstantBuffers(2, 1, m_ProjectionBuffer.GetAddressOf());

    bufferDesc.ByteWidth = sizeof(MATERIAL);
    m_Device->CreateBuffer(&bufferDesc, nullptr, m_MaterialBuffer.GetAddressOf());
    m_DeviceContext->VSSetConstantBuffers(3, 1, m_MaterialBuffer.GetAddressOf());
    m_DeviceContext->PSSetConstantBuffers(3, 1, m_MaterialBuffer.GetAddressOf());

    bufferDesc.ByteWidth = sizeof(LIGHT);
    m_Device->CreateBuffer(&bufferDesc, nullptr, m_LightBuffer.GetAddressOf());
    m_DeviceContext->VSSetConstantBuffers(4, 1, m_LightBuffer.GetAddressOf());
    m_DeviceContext->PSSetConstantBuffers(4, 1, m_LightBuffer.GetAddressOf());
}

/**
 * @brief �g�p���Ă������\�[�X��S�ĉ�����܂��B
 *
 * @details
 * Direct3D�̃��\�[�X�͖����I�ɉ�����Ȃ��ƃ��������[�N���������邽�߁A
 * ComPtr::Reset()�ň��S�Ƀ��\�[�X���J�����Ă��܂��B
 */
void Renderer::Uninit()
{
    for (auto& bs : m_BlendState) {
        bs.Reset();
    }
    m_BlendStateATC.Reset();
    m_DepthStateEnable.Reset();
    m_DepthStateDisable.Reset();
    m_WorldBuffer.Reset();
    m_ViewBuffer.Reset();
    m_ProjectionBuffer.Reset();
    m_LightBuffer.Reset();
    m_MaterialBuffer.Reset();
    m_RenderTargetView.Reset();
    m_SwapChain.Reset();
    m_DeviceContext.Reset();
    m_Device.Reset();
}

/**
 * @brief 1�t���[���̕`����J�n���܂��B
 *
 * @details
 * - ��ʂ��w��F�i�F�j�ŃN���A
 * - �[�x�o�b�t�@��������
 *
 * ���t���[���K���Ăяo���āA�O�̃t���[���̎c���������܂��B
 */
void Renderer::Begin()
{
    float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

/**
 * @brief �`����I�����āA��ʂɕ\�����܂��B
 *
 * @details
 * Present�Ńo�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ��܂��B
 */
void Renderer::End()
{
    m_SwapChain->Present(1, 0);
}

/**
 * @brief �[�x�e�X�g�iZ�o�b�t�@�j�̗L��/������؂�ւ��܂��B
 * @param Enable true�Ȃ�L���Afalse�Ȃ疳��
 * @details
 * �[�x�e�X�g�́A���ɂ�����̂𐳂�����O�̂��̗̂��ɕ`�悷�邽�߂̋@�\�ł��B
 */
void Renderer::SetDepthEnable(bool Enable)
{
    m_DeviceContext->OMSetDepthStencilState(
        Enable ? m_DepthStateEnable.Get() : m_DepthStateDisable.Get(), 0);
}

/**
 * @brief Alpha To Coverage�i�������\���p�j��ON/OFF��؂�ւ��܂��B
 * @param Enable true�Ȃ�ATC�L���Afalse�Ȃ疳��
 * @details
 * �}���`�T���v�����O�{�A���t�@�u�����h�̍��x�ȍ������s���@�\�ł��B
 */
void Renderer::SetATCEnable(bool Enable)
{
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_DeviceContext->OMSetBlendState(
        Enable ? m_BlendStateATC.Get() : m_BlendState[0].Get(),
        blendFactor, 0xffffffff);
}

/**
 * @brief 2D�`��p�ɁA�P���ȃ��[���h�E�r���[�E�v���W�F�N�V�����s����Z�b�g���܂��B
 *
 * @details
 * ��ʍ�������_�Ƃ���2D���𓊉e�s��𐶐����A�e�s��o�b�t�@�ɐݒ肵�܂��B
 */
void Renderer::SetWorldViewProjection2D()
{
    Matrix4x4 world = Matrix4x4::Identity.Transpose();
    m_DeviceContext->UpdateSubresource(m_WorldBuffer.Get(), 0, nullptr, &world, 0, 0);

    Matrix4x4 view = Matrix4x4::Identity.Transpose();
    m_DeviceContext->UpdateSubresource(m_ViewBuffer.Get(), 0, nullptr, &view, 0, 0);

    Matrix4x4 projection = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f,
        static_cast<float>(Application::GetWidth()),
        static_cast<float>(Application::GetHeight()),
        0.0f,
        0.0f,
        1.0f);
    projection = projection.Transpose();
    m_DeviceContext->UpdateSubresource(m_ProjectionBuffer.Get(), 0, nullptr, &projection, 0, 0);
}

/**
 * @brief �C�ӂ̃��[���h�s����V�F�[�_�[�ɃZ�b�g���܂��B
 * @param WorldMatrix ���[���h�s��ւ̃|�C���^
 */
void Renderer::SetWorldMatrix(Matrix4x4* WorldMatrix)
{
    Matrix4x4 mat = WorldMatrix->Transpose();
    m_DeviceContext->UpdateSubresource(m_WorldBuffer.Get(), 0, nullptr, &mat, 0, 0);
}

/**
 * @brief �C�ӂ̃r���[�s����V�F�[�_�[�ɃZ�b�g���܂��B
 * @param ViewMatrix �r���[�s��ւ̃|�C���^
 */
void Renderer::SetViewMatrix(Matrix4x4* ViewMatrix)
{
    Matrix4x4 mat = ViewMatrix->Transpose();
    m_DeviceContext->UpdateSubresource(m_ViewBuffer.Get(), 0, nullptr, &mat, 0, 0);
}

/**
 * @brief �C�ӂ̃v���W�F�N�V�����s����V�F�[�_�[�ɃZ�b�g���܂��B
 * @param ProjectionMatrix �ˉe�s��ւ̃|�C���^
 */
void Renderer::SetProjectionMatrix(Matrix4x4* ProjectionMatrix)
{
    Matrix4x4 mat = ProjectionMatrix->Transpose();
    m_DeviceContext->UpdateSubresource(m_ProjectionBuffer.Get(), 0, nullptr, &mat, 0, 0);
}

/**
 * @brief �}�e���A���i�\�ʍގ��j�����Z�b�g���܂��B
 * @param Material �}�e���A�����
 */
void Renderer::SetMaterial(MATERIAL Material)
{
    m_DeviceContext->UpdateSubresource(m_MaterialBuffer.Get(), 0, nullptr, &Material, 0, 0);
}

/**
 * @brief ���C�g�i�����j�����Z�b�g���܂��B
 * @param Light ���C�g���
 */
void Renderer::SetLight(LIGHT Light)
{
    m_DeviceContext->UpdateSubresource(m_LightBuffer.Get(), 0, nullptr, &Light, 0, 0);
}

/**
 * @brief �w�肵���u�����h�X�e�[�g���Z�b�g���܂��B
 * @param nBlendState �g�p����u�����h�X�e�[�g�̎��
 */
void Renderer::SetBlendState(int nBlendState)
{
    if (nBlendState >= 0 && nBlendState < MAX_BLENDSTATE) {
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_DeviceContext->OMSetBlendState(m_BlendState[nBlendState].Get(), blendFactor, 0xffffffff);
    }
}

/**
 * @brief �ʂ̏��O�i�J�����O�j�𖳌��܂��͗L���ɂ��܂��B
 * @param cullflag true�ŃJ�����OON�i�ʏ�j�Afalse�ŃJ�����OOFF�i���ʕ`��j
 */
void Renderer::DisableCulling(bool cullflag)
{
    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = cullflag ? D3D11_CULL_BACK : D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    ComPtr<ID3D11RasterizerState> pRasterizerState;
    HRESULT hr = m_Device->CreateRasterizerState(&rasterizerDesc, pRasterizerState.GetAddressOf());
    if (FAILED(hr))
        return;

    m_DeviceContext->RSSetState(pRasterizerState.Get());
}


/**
 * @brief ���X�^���C�U�X�e�[�g�̃t�B�����[�h�i�h��Ԃ�/���C���[�t���[���j��ݒ肵�܂��B
 * @param FillMode D3D11_FILL_SOLID �܂��� D3D11_FILL_WIREFRAME
 */
void Renderer::SetFillMode(D3D11_FILL_MODE FillMode)
{
    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = FillMode;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.MultisampleEnable = FALSE;

    ComPtr<ID3D11RasterizerState> rs;
    m_Device->CreateRasterizerState(&rasterizerDesc, rs.GetAddressOf());
    m_DeviceContext->RSSetState(rs.Get());
}

/**
 * @brief �[�x�e�X�g����Ƀp�X������ݒ�ɕύX���܂��B
 *
 * @details
 * - �[�x�e�X�g�͗L���iDepthEnable = TRUE�j
 * - �������A��Ɂu�`��OK�v�iDepthFunc = D3D11_COMPARISON_ALWAYS�j
 * - �[�x�o�b�t�@�ɂ��������ށiDepthWriteMask = ALL�j
 */
void Renderer::SetDepthAllwaysWrite()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; // ��ɐ[�x�e�X�g����
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.StencilEnable = FALSE; // �X�e���V���e�X�g�͖���

    ComPtr<ID3D11DepthStencilState> pDepthStencilState;
    HRESULT hr = m_Device->CreateDepthStencilState(&depthStencilDesc, pDepthStencilState.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        m_DeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0);
    }
}
