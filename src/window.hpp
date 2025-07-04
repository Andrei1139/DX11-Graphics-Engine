#include <QApplication>
#include <QWindow>
#include <qnamespace.h>
#include "renderer.hpp"

class Window: public QWindow {
    public:
        Window(int width, int height, QWindow *parent = nullptr):
        QWindow(parent), renderer(*this, width, height) {
            setWidth(width);
            setHeight(height);
        }
        virtual ~Window() {}

        void paintEvent(QPaintEvent *) override {renderer.render();}
    private:
        Renderer renderer;
};