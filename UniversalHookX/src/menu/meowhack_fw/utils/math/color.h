#pragma once
#include <cmath>

class color {
public:
    int r, g, b, a = 255;

    color() : r(255), g(255), b(255), a(255) {};

    color(int alpha) : r(255), g(255), b(255), a(alpha) {};

    color(int red, int green, int blue) : r(red), g(green), b(blue), a(255) {};

    color(int red, int green, int blue, int alpha) : r(red), g(green), b(blue), a(alpha) {};

    static color white() { return color(); };
    static color black() { return color(0, 0, 0); };
    static color red() { return color(255, 0, 0); };
    static color green() { return color(0, 255, 0); };
    static color blue() { return color(0, 0, 255); };

    // !replaces alpha
    color adjust_alpha(int alpha) {
        return color(r, g, b, alpha);
    }

    // !multiplies alpha
    color adjust_alpha(float add) {
        return color(r, g, b, a * add);
    }

    ImColor imgui() {
        return ImColor(r, g, b, a);
    }

    color lerp(color new_color, float additive) {
        const int new_r = std::lerp(this->r, new_color.r, additive);
        const int new_g = std::lerp(this->g, new_color.g, additive);
        const int new_b = std::lerp(this->b, new_color.b, additive);
        const int new_a = std::lerp(this->a, new_color.a, additive);
        return color(new_r, new_g, new_b, new_a);
    }
};
