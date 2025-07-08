#include <d3d11.h>
#include <minwindef.h>
#include <QWindow>
#include "renderer.hpp"
#include "graphicsEngine.hpp"

Renderer::Renderer(const QWindow& window, int width, int height, Camera &camera) {
    engine = std::make_unique<GraphicsEngine>(window, width, height, camera);
    Triangle triangle;
    triangles.push_back(triangle);
}

void Renderer::render() {
    engine->refreshPipeline();
    engine->display();
    engine->reset();
}

void Renderer::drawObject(Triangle &triangle) {
    engine->addTriangle(triangle);
}

