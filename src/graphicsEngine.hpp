#pragma once
#include "camera.hpp"
#undef _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
#include <QApplication>
#include <QWidget>
#include <windows.h>
#include <winnt.h>
#include "Triangle.hpp"

class GraphicsEngine {
    public:
        GraphicsEngine(const QWindow& window, int width, int height, Camera &camera);
        GraphicsEngine(const GraphicsEngine&) = delete;
        ~GraphicsEngine() {
            swapChain->Release();
            device->Release();
            deviceContext->Release();
            backBufferTargetView->Release();
            depthStencilView->Release();

            vShader->Release();
            vShaderBlob->Release();
            pShader->Release();
        }
        void fillScreen(float r, float g, float b);
        void display();
        void refreshPipeline();

        void reset();
        void addTriangle(Triangle& triangle);

    private:
        std::vector<TriangleData> trianglesData;

        void initSwapChainData(const QWindow& window);
        void initDeviceAndSwapChain();
        void initBackBuffer();
        void initDepthStencil();
        void setupViewport();
        void compileVertexShader();
        void compilePixelShader();
        void initShaderCTBuffer();

        void initVertexBuffer();
        void setupInputAssembler();
        void setupRasterizer();

        DXGI_SWAP_CHAIN_DESC swapChainData;
        IDXGISwapChain *swapChain;
        ID3D11Device *device;
        ID3D11DeviceContext *deviceContext;

        ID3D11RenderTargetView *backBufferTargetView;
        
        ID3D11DepthStencilView *depthStencilView;
        ID3D11Texture2D *depthStencilTexture;

        ID3D11Buffer *shaderCTBuffer;

        ID3D11Buffer *vertexBuffer = nullptr;
        ID3D11VertexShader *vShader;
        ID3DBlob *vShaderBlob;

        ID3D11PixelShader *pShader;

        DirectX::XMMATRIX viewProjMat;

        int width, height;
        Camera &camera;
};