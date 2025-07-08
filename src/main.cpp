#include <QApplication>
#include <qnamespace.h>
#include <vector>
#include <QTimer>
#include <iostream>
#include "renderer.hpp"
#include "window.hpp"
#include "camera.hpp"

constexpr int width = 1280, height = 720;

int main(int argc, char **argv) {
    QApplication app(argc, argv);


    std::vector<Triangle> triangles(1);

    Camera camera;

    Window window(width, height);
    window.show();

    Renderer renderer(window, width, height, camera);

    QCursor::setPos(window.mapToGlobal(QPoint(width / 2, height / 2)));
    app.setOverrideCursor(QCursor(Qt::BlankCursor));

    auto oneLoop = [&]() {
        auto pos = window.mapFromGlobal(QCursor::pos());
        auto dx = width / 2 - pos.x(), dy = height / 2 - pos.y();
        QCursor::setPos(window.mapToGlobal(QPoint(width / 2, height / 2)));

        camera.adjustH(-dx);
        camera.adjustV(dy);

        if (window.clickPressed) {
            Triangle triangle;
            triangles.push_back(triangle);
            window.clickPressed = false;
        }
        if (window.keyUp) {
            camera.moveUp();
        }
        if (window.keyDown) {
            camera.moveDown();
        }
        if (window.keyLeft) {
            camera.moveLeft();
        }
        if (window.keyRight) {
            camera.moveRight();
        }
        if (window.keyFwd) {
            camera.moveForward();
        }
        if (window.keyBack) {
            camera.moveBackward();
        }

        for (Triangle &triangle: triangles) {
            triangle.update();
            renderer.drawObject(triangle);
        }

        renderer.render();
    };

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, oneLoop);
    timer.start(1000/60);

    return app.exec();
}