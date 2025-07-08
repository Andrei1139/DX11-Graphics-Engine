#pragma once
#include <d3d11.h>
#include <QApplication>
#include <QWidget>
#include <windows.h>
#include <memory.h>
#include "camera.hpp"
#include "graphicsEngine.hpp"

class Renderer {
    public:
        Renderer(const QWindow& window, int width, int height, Camera &camera);
        Renderer(const Renderer&) = delete;
        void render();
        void drawObject(Triangle &triangle);

    private:
        std::unique_ptr<GraphicsEngine> engine;
        std::vector<Triangle> triangles;
};