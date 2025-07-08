#pragma once
#include <DirectXMath.h>
#include "utils.hpp"

struct Vertex {
    DirectX::XMFLOAT3 coords;
    DirectX::XMFLOAT3 colors;
};

struct TriangleData {
    Vertex v0, v1, v2;

    Vertex& operator[](int i) {
        switch (i) {
            case 0: return v0;
            case 1: return v1;
            case 2: return v2;
        }
        return v0;
    }
};

struct Triangle {
    public:
        Triangle &set(int index, bool pos, float x, float y, float z) {
            if (pos) {
                triangleData[index].coords = DirectX::XMFLOAT3(x, y, z);
            } else {
                triangleData[index].colors = DirectX::XMFLOAT3(x, y, z);
            }
            return *this;
        }

        TriangleData triangleData = {
            {DirectX::XMFLOAT3( 0.0f, 5.0f, 1.0f ),
            // DirectX::XMFLOAT3( get_random_float(), get_random_float(), get_random_float() )},
            DirectX::XMFLOAT3( 1, 0, 0 )},
            {DirectX::XMFLOAT3( 5.0f, -5.0f, 1.0f ),
            DirectX::XMFLOAT3( 0, 1, 0 )},
            {DirectX::XMFLOAT3( -5.0f, -5.0f, 1.0f ),
            DirectX::XMFLOAT3( 0, 0, 1 )},
        };
        // float dx = (2 * get_random_float() - 1) / 100;
        // float dy = (2 * get_random_float() - 1) / 100;
        float cos = 0.9998;
        float sin = 0.0175;

        void update() {
            // for (auto i = 0; i < 3; ++i) {
            //     triangleData[i].coords.x += dx;
            //     triangleData[i].coords.y += dy;
            // }
            // if (triangleData[0].coords.y >= 1.00 || 
            //     triangleData[0].coords.y <= -1.00) dy *= -1;
            for (auto i = 0; i < 3; ++i) {
                triangleData[i].coords.x = cos * triangleData[i].coords.x - sin * triangleData[i].coords.y;
                triangleData[i].coords.y = sin * triangleData[i].coords.x + cos * triangleData[i].coords.y;
            }

        }
};