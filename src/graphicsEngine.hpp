#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
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
            backBufferTargetView->Release();
            depthStencilView->Release();
        }
        void fillScreen(float r, float g, float b);
        void drawTriangle();
        void display();

    private:
        using Vertex = struct {
            DirectX::XMFLOAT3 coords;
            DirectX::XMFLOAT3 colors;
        }; 
        Vertex triangleData[3] = {
            {DirectX::XMFLOAT3( 0.0f, 0.5f, 0.1f ),
            DirectX::XMFLOAT3( 0.0f, 0.0f, 0.5f )},
            {DirectX::XMFLOAT3( 0.5f, -0.5f, 0.1f ),
            DirectX::XMFLOAT3( 0.5f, 0.0f, 0.0f )},
            {DirectX::XMFLOAT3( -0.5f, -0.5f, 0.1f ),
            DirectX::XMFLOAT3( 0.0f, 0.5f, 0.0f )},
        };

        void initSwapChainData(const QWindow& window);
        void initDeviceAndSwapChain();
        void initBackBuffer();
        void initDepthStencil();
        void setupViewport();

        void initVertexBuffer();
        void aux();

        DXGI_SWAP_CHAIN_DESC swapChainData;
        IDXGISwapChain *swapChain;
        ID3D11Device *device;
        ID3D11DeviceContext *deviceContext;

        ID3D11RenderTargetView *backBufferTargetView;
        
        ID3D11DepthStencilView *depthStencilView;
        ID3D11Texture2D *depthStencilBuffer;

        ID3D11Buffer *vertexBuffer;

        int width, height;
};