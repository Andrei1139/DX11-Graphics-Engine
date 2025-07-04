#pragma once
#include <d3d11.h>
#include <QApplication>
#include <QWidget>
#include <windows.h>
#include "graphicsEngine.hpp"

class Renderer {
    public:
        Renderer(const QWindow& window, int width, int height);
        Renderer(const Renderer&) = delete;
        void render();

    private:
        GraphicsEngine engine;
};