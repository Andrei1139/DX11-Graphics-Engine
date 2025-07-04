#include "graphicsEngine.hpp"
#include <d3d11.h>
#include <minwindef.h>
#include <iostream>
#include <QWindow>

GraphicsEngine::GraphicsEngine(const QWindow& window, int width, int height): width{width}, height{height} {
    initSwapChainData(window);
    initDeviceAndSwapChain();
    initBackBuffer();
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
    ID3D11Texture2D *backBufferAddr;
    auto result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID *>(&backBufferAddr));
    if (FAILED(result)) {
        std::cerr << "ERROR ON GetBuffer\n";
    }

    result = device->CreateRenderTargetView(backBufferAddr, NULL, &backBuffer);
    if (FAILED(result)) {
        std::cerr << "ERROR ON CreateRenderTargetView\n";
    }
    backBufferAddr->Release();

    deviceContext->OMSetRenderTargets(1, &backBuffer, NULL);
}

void GraphicsEngine::setupViewport() {
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
    deviceContext->ClearRenderTargetView(backBuffer, color);
}

void GraphicsEngine::display() {
    // Swap front buffer with back buffer
    auto result = swapChain->Present(0, 0);
    if (FAILED(result)) {
        std::cerr << "ERROR ON Present\n";
    }
}

