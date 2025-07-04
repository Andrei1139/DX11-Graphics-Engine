#include <QApplication>
#include <qnamespace.h>
#include "window.hpp"

constexpr int width = 1280, height = 720;

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    Window window(width, height);
    window.show();

    return app.exec();
}