#pragma once
#include <DirectXMath.h>
#include <cmath>
#include <qmath.h>

class Camera {
    public:
        void moveUp() {y += movement;}
        void moveDown() {y -= movement;}
        void moveForward() {z += movement * cos(hDegrees); x += movement * sin(hDegrees);}
        void moveBackward() {z -= movement * cos(hDegrees); x -= movement * sin(hDegrees);}
        void moveLeft() {x -= movement * cos(hDegrees); z += movement * sin(hDegrees);}
        void moveRight() {x += movement * cos(hDegrees); z -= movement * sin(hDegrees);}

        void adjustV(float degrees) {
            vDegrees += degrees / amortization;
            if (vDegrees < -M_PI) vDegrees = -M_PI;
            else if (vDegrees > M_PI) vDegrees = M_PI;
        }

        void adjustH(float degrees) {
            hDegrees += degrees / amortization;
            if (hDegrees < -2 * M_PI || hDegrees > 2 * M_PI) hDegrees = 0; 
        }

        DirectX::XMVECTOR getPos() {return {x, y, z};}
        DirectX::XMVECTOR getDir() {
            auto cosV = cos(vDegrees);
            return {sin(hDegrees) * cosV, sin(vDegrees), cos(hDegrees) * cos(vDegrees)};
        }
    private:
        float x = 0, y = 0, z = -2;
        float vDegrees = 0, hDegrees = 0;
        const float amortization = 1000, movement = 0.7;
};