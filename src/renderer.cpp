#include <d3d11.h>
#include <minwindef.h>
#include <QWindow>
#include "renderer.hpp"

Renderer::Renderer(const QWindow& window, int width, int height): engine(window, width, height) {}

void Renderer::render() {
    engine.fillScreen(1, 0, 0);
    engine.display();
}

