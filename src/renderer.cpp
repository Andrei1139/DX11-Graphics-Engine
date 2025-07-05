#include <d3d11.h>
#include <minwindef.h>
#include <QWindow>
#include "renderer.hpp"

Renderer::Renderer(const QWindow& window, int width, int height): engine(window, width, height) {}

void Renderer::render() {
    static float r = 0;
    engine.fillScreen(r / 1000.0, 0, 0);
    engine.display();
    ++r;
    if (r == 1000) r = 0;
}

