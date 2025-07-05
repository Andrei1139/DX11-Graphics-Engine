#include <QApplication>
#include <QWindow>
#include <qnamespace.h>
#include <QTimer>
#include <QRasterWindow>
#include <qrasterwindow.h>
#include <memory.h>
#include "renderer.hpp"

static void updateWnd();

static std::unique_ptr<Renderer> renderer;

class Window: public QWindow {
    public:
        Window(int width, int height, QWindow *parent = nullptr):
        QWindow(parent), timer(this){
            setWidth(width);
            setHeight(height);
            renderer = std::make_unique<Renderer>(*this, width, height);

            connect(&timer, &QTimer::timeout, updateWnd);
            timer.start(1000/60);
        }
        virtual ~Window() {}
    private:
        QTimer timer;
};

static void updateWnd() {
    renderer->render();
}