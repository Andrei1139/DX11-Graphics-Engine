#include "graphicsEngine.hpp"
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d3dcommon.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <iostream>
#include <QWindow>
#include <winerror.h>

GraphicsEngine::GraphicsEngine(const QWindow& window, int width, int height): width{width}, height{height} {
    initSwapChainData(window);
    initDeviceAndSwapChain();
    initBackBuffer();
    initDepthStencil();
    setupViewport();
}

void GraphicsEngine::initSwapChainData(const QWindow& window) {
    // Get window handle from Qt API
    HWND windowHandle = reinterpret_cast<HWND>(window.winId());

    ZeroMemory(&swapChainData, sizeof(swapChainData));

    swapChainData.BufferCount = 1; // One single back buffer
    swapChainData.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainData.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainData.OutputWindow = windowHandle;
    swapChainData.SampleDesc.Count = 1;
    swapChainData.SampleDesc.Quality = 0;
    swapChainData.Windowed = TRUE;
}

void GraphicsEngine::initDeviceAndSwapChain() {
    // Initialize device, device context and swap chain
    auto result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
        NULL, 0, NULL, 0, D3D11_SDK_VERSION, &swapChainData,
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
    D3D11_TEXTURE2D_DESC depthStencilData;
    ZeroMemory(&depthStencilData, sizeof(depthStencilData));

    depthStencilData.Width = width;
    depthStencilData.Height = height;
    depthStencilData.ArraySize = depthStencilData.MipLevels = 1;
    depthStencilData.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilData.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilData.Usage = D3D11_USAGE_DEFAULT;
    depthStencilData.SampleDesc.Count = 1;

    device->CreateTexture2D(&depthStencilData, NULL, &depthStencilBuffer);
    device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
}

void GraphicsEngine::setupViewport() {
    // Bind render target and depth stencil view to output-merger stage of pipeline
    deviceContext->OMSetRenderTargets(1, &backBufferTargetView, depthStencilView);

    // Establish relation between normal coordinates and DirectX coordinates
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(viewport));

    viewport.TopLeftX = viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;

    deviceContext->RSSetViewports(1, &viewport);
}

void GraphicsEngine::fillScreen(float r, float g, float b) {
    FLOAT color[] = {r, g, b, 1.0};
    deviceContext->ClearRenderTargetView(backBufferTargetView, color);
}

void GraphicsEngine::display() {
    // Swap front buffer with back buffer
    auto result = swapChain->Present(0, 0);
    if (FAILED(result)) {
        std::cerr << "ERROR ON Present\n";
    }
}

void GraphicsEngine::initVertexBuffer() {
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = bufferDesc.MiscFlags = 0;
    bufferDesc.ByteWidth = sizeof(Vertex) * 3;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = triangleData;
    initData.SysMemPitch = initData.SysMemSlicePitch = 0;

    auto result = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateBuffer\n";
    }
}

void GraphicsEngine::aux() {
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    ID3D11InputLayout *inputLayoutPtr;

    device->CreateInputLayout(layout, 2, NULL /*TODO*/, 0 /*TODO*/, &inputLayoutPtr);

    UINT strides = sizeof(Vertex), offsets = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
    deviceContext->IASetInputLayout(inputLayoutPtr);

    deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->Draw(3, 0);
}

