#include <QApplication>
#include <QWindow>
#include <qnamespace.h>
#include <QTimer>
#include <QRasterWindow>
#include <qrasterwindow.h>
#include <QKeyEvent>
#include <memory.h>
#include <qwindow.h>

static void updateWnd();

class Window: public QWindow {
    public:
        Window(int width, int height, QWindow *parent = nullptr):
        QWindow(parent), timer(this){
            setWidth(width);
            setHeight(height);

            
        }
        virtual ~Window() {}

        void mousePressEvent(QMouseEvent *) override {
            clickPressed = true;
        }

        void keyPressEvent(QKeyEvent *event) override {
            switch (event->key()) {
                case Qt::Key_W:
                    keyFwd = true;
                    break;
                case Qt::Key_S:
                    keyBack = true;
                    break;
                case Qt::Key_A:
                    keyLeft = true;
                    break;
                case Qt::Key_D:
                    keyRight = true;
                    break;
                case Qt::Key_Space:
                    keyUp = true;
                    break;
                case Qt::Key_Shift:
                    keyDown = true;
                    break;
            }
        }

        void keyReleaseEvent(QKeyEvent *event) override {
            switch (event->key()) {
                case Qt::Key_W:
                    keyFwd = false;
                    break;
                case Qt::Key_S:
                    keyBack = false;
                    break;
                case Qt::Key_A:
                    keyLeft = false;
                    break;
                case Qt::Key_D:
                    keyRight = false;
                    break;
                case Qt::Key_Space:
                    keyUp = false;
                    break;
                case Qt::Key_Shift:
                    keyDown = false;
                    break;
            }
        }

        // void mouseMoveEvent(QMouseEvent *event) override {
        //     event->
        // }

        bool clickPressed = false, keyUp = false, keyDown = false, keyLeft = false, keyRight = false, keyFwd = false, keyBack = false;
    private:
        QTimer timer;
};