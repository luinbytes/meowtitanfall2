/*
    COPYRIGHT : Carbon Solutions

    this documented is protected by the "Carbon Solutions" Copyright, any material included within this document is only allowed to be used
    and abused by Ichigo & GhosT (the femboy)

    Any of "Carbon Solutions" material used outside of this project will be prosecuted fairly.

*/

#pragma once

class vector2f {
public:
    float x, y = 0.f;

    vector2f() : x(0.f), y(0.f) {};

    vector2f(float a) : x(a), y(a) {};

    vector2f(float _x, float _y) : x(_x), y(_y) {};

    vector2f operator+(vector2f other) {
        return vector2f(this->x + other.x, this->y + other.y);
    }

    vector2f operator+(float other) {
        return vector2f(this->x + other, this->y + other);
    }

    vector2f operator-(vector2f other) {
        return vector2f(this->x - other.x, this->y - other.y);
    }

    vector2f operator-(float other) {
        return vector2f(this->x - other, this->y - other);
    }

    vector2f operator/(vector2f other) {
        return vector2f(this->x / other.x, this->y / other.y);
    }

    vector2f operator/(float other) {
        return vector2f(this->x / other, this->y / other);
    }

    vector2f operator*(vector2f other) {
        return vector2f(this->x * other.x, this->y * other.y);
    }

    vector2f operator*(float other) {
        return vector2f(this->x * other, this->y * other);
    }

    ImVec2 imgui() {
        return ImVec2(x, y);
    }
};
