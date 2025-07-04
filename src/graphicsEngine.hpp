#pragma once
#include <d3d11.h>
#include <QApplication>
#include <QWidget>
#include <windows.h>

class GraphicsEngine {
    public:
        GraphicsEngine(const QWindow& window, int width, int height);
        GraphicsEngine(const GraphicsEngine&) = delete;
        ~GraphicsEngine() {
            swapChain->Release();
            device->Release();
            deviceContext->Release();
            backBuffer->Release();
        }
        void fillScreen(float r, float g, float b);
        void display();

    private:
        void initSwapChainData(const QWindow& window);
        void initDeviceAndSwapChain();
        void initBackBuffer();
        void setupViewport();

        DXGI_SWAP_CHAIN_DESC swapChainData;
        IDXGISwapChain *swapChain;
        ID3D11Device *device;
        ID3D11DeviceContext *deviceContext;

        ID3D11RenderTargetView *backBuffer;

        int width, height;
};