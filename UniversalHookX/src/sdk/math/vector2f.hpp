
class vector2f {
public:
    float x = 0.f, y = 0.f;

    vector2f( ) = default;

    vector2f(float additive) : x(additive), y(additive){ };

    vector2f(float _x, float _y) : x(_x), y(_y){ };

    vector2f operator+(vector2f other) {
        return vector2f(this->x + other.x, this->y + other.y);
    }

    vector2f operator-(vector2f other) {
        return vector2f(this->x - other.x, this->y - other.y);
    }

    vector2f operator+(float other) {
        return vector2f(this->x + other, this->y + other);
    }

    vector2f operator-(float other) {
        return vector2f(this->x - other, this->y - other);
    }

    vector2f operator/(vector2f other) {
        return vector2f(this->x / other.x, this->y / other.y);
    }

    vector2f operator*(vector2f other) {
        return vector2f(this->x * other.x, this->y * other.y);
    }

    vector2f operator/(float other) {
        return vector2f(this->x / other, this->y / other);
    }

    vector2f operator*(float other) {
        return vector2f(this->x * other, this->y * other);
    }
};
