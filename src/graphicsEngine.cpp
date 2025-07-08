#include "graphicsEngine.hpp"
#include <DirectXMath.h>
#include <cstring>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <iostream>
#include <QWindow>
#include <qsharedpointer.h>
#include <winerror.h>

GraphicsEngine::GraphicsEngine(const QWindow& window, int width, int height, Camera &camera):
width{width}, height{height}, camera{camera} {
    initSwapChainData(window);
    initDeviceAndSwapChain();
    initBackBuffer();
    initDepthStencil();
    setupViewport();

    compileVertexShader();
    compilePixelShader();
    setupRasterizer();
}

void GraphicsEngine::initSwapChainData(const QWindow& window) {
    // Get window handle from Qt API
    HWND windowHandle = reinterpret_cast<HWND>(window.winId());

    ZeroMemory(&swapChainData, sizeof(swapChainData));

    swapChainData.BufferCount = 1; // One single back buffer
    swapChainData.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainData.BufferDesc.Height = height;
    swapChainData.BufferDesc.Width = width;
    swapChainData.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainData.OutputWindow = windowHandle;
    swapChainData.SampleDesc.Count = 1;
    swapChainData.SampleDesc.Quality = 0;
    swapChainData.Windowed = TRUE;
}

void GraphicsEngine::initDeviceAndSwapChain() {
    UINT flags = 0;
    #ifdef _DEBUG
    flags = D3D11_CREATE_DEVICE_DEBUG;
    #endif

    // Initialize device, device context and swap chain
    auto result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
        NULL, flags, NULL, 0, D3D11_SDK_VERSION, &swapChainData,
        &swapChain, &device, NULL, &deviceContext);

    if (FAILED(result)) {
        std::cerr << "ERROR ON D3D11CreateDeviceAndSwapChain\n";
    }
}

void GraphicsEngine::initBackBuffer() {
    // Get the back buffer and create a render target out of it
    ID3D11Texture2D *backBuffer;
    auto result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID *>(&backBuffer));
    if (FAILED(result)) {
        std::cerr << "ERROR ON GetBuffer\n";
    }

    result = device->CreateRenderTargetView(backBuffer, NULL, &backBufferTargetView);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateRenderTargetView\n";
    }
    backBuffer->Release();
}

void GraphicsEngine::initDepthStencil() {
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;


    auto result = device->CreateTexture2D(&descDepth, NULL, &depthStencilTexture);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateTexture2D\n";
    }

    // Now setting up the depth stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* depthState;
    result = device->CreateDepthStencilState(&dsDesc, &depthState);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateDepthStencilState\n";
    }

    deviceContext->OMSetDepthStencilState(depthState, 1);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    descDSV.Flags = 0;

    result = device->CreateDepthStencilView(depthStencilTexture, &descDSV, &depthStencilView);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateDepthStencilView\n";
    }
}

void GraphicsEngine::setupViewport() {
    // Establish relation between normal coordinates and DirectX coordinates
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(viewport));

    viewport.TopLeftX = viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;

    deviceContext->RSSetViewports(1, &viewport);
}

void GraphicsEngine::initShaderCTBuffer() {
    auto viewMat = DirectX::XMMatrixLookToLH(camera.getPos(), camera.getDir(), {0, 1, 0});
    auto projMat = DirectX::XMMatrixPerspectiveFovLH(1, (float)width / height, 1, 1000);

    viewProjMat = DirectX::XMMatrixMultiply(viewMat, projMat);


    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.ByteWidth = sizeof(viewProjMat);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &viewProjMat;
    initData.SysMemPitch = initData.SysMemSlicePitch = 0;

    auto result = device->CreateBuffer(&bufferDesc, &initData, &shaderCTBuffer);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateBuffer\n";
    }
}


void GraphicsEngine::initVertexBuffer() {
    if (vertexBuffer) {
        vertexBuffer->Release();
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.MiscFlags = 0;
    bufferDesc.ByteWidth = sizeof(Vertex) * 3 * trianglesData.size();
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = trianglesData.data();
    initData.SysMemPitch = initData.SysMemSlicePitch = 0;
    
    auto result = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateBuffer\n";
    }
}

void GraphicsEngine::compileVertexShader() {
    auto result = D3DCompileFromFile(L"..//..//..//src//vShader.hlsl", NULL, NULL, "main", "vs_5_0", 0, 0, &vShaderBlob, NULL);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CompileFromFile\n";
    }
    result = device->CreateVertexShader(vShaderBlob->GetBufferPointer(), vShaderBlob->GetBufferSize(), NULL, &vShader);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateShader\n";
    }
}

void GraphicsEngine::compilePixelShader() {
    ID3DBlob *pShaderBlob;
    ID3DBlob *errorMsg;
    auto result = D3DCompileFromFile(L"..//..//..//src//pShader.hlsl", NULL, NULL, "main", "ps_5_0", 0, 0, &pShaderBlob, &errorMsg);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CompileFromFile\n";
        std::cerr << (const char *)(errorMsg->GetBufferPointer()) << "\n";
        errorMsg->Release();
    }
    result = device->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &pShader);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateShader\n";
    }
}


void GraphicsEngine::setupInputAssembler() {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    static ID3D11InputLayout *inputLayoutPtr;
    
    static auto result = device->CreateInputLayout(layout, 2, vShaderBlob->GetBufferPointer(), vShaderBlob->GetBufferSize(), &inputLayoutPtr);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateInputLayout\n";
    }
    
    UINT strides = sizeof(Vertex), offsets = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
    deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->IASetInputLayout(inputLayoutPtr);
}

void GraphicsEngine::setupRasterizer() {
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.DepthClipEnable = true;

    ID3D11RasterizerState *rasterizerState;
    device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    deviceContext->RSSetState(rasterizerState);
} 

void GraphicsEngine::display() {
    deviceContext->Draw(3 * trianglesData.size(), 0);
    // Swap front buffer with back buffer
    auto result = swapChain->Present(0, 0);
    if (FAILED(result)) {
        std::cerr << "ERROR ON Present\n";
    }
}

void GraphicsEngine::refreshPipeline() {
    float color[] = {0.0, 0.0, 0.15, 1.0};
    deviceContext->ClearRenderTargetView(backBufferTargetView, color);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

    deviceContext->OMSetRenderTargets(1, &backBufferTargetView, depthStencilView);

    deviceContext->VSSetShader(vShader, NULL, 0);
    deviceContext->PSSetShader(pShader, NULL, 0);
    
    setupViewport();
    
    initShaderCTBuffer();
    initVertexBuffer();
    setupInputAssembler();

    deviceContext->VSSetConstantBuffers(0,1, &shaderCTBuffer);

    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    ZeroMemory(&mappedSubresource, sizeof(mappedSubresource));
    deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, trianglesData.data(), trianglesData.size() * sizeof(trianglesData[0]));
    deviceContext->Unmap(vertexBuffer, 0);
}

void GraphicsEngine::reset() {
    trianglesData.clear();
}

void GraphicsEngine::addTriangle(Triangle& triangle) {
    trianglesData.push_back(triangle.triangleData);
}