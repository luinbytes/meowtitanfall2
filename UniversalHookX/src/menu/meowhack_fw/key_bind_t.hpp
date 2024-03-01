#include <windows.h>

enum e_keybind_state : int {
    always,
    toggled,
    hold,

    key_max
};

struct key_bind_t {
    int key;
    int mode;

    key_bind_t( ) {
        this->key = 0;
        this->mode = hold;
    }

    key_bind_t(int _key, int _mode) {
        this->key = _key;
        this->mode = _mode;
    }

    bool get_state( ) {
        switch (mode) {
            case always:
                return true;
                break;
            case toggled: {
                if (key <= 0) {
                    return false;
                }
                if (GetAsyncKeyState(key) & 1) {
                    this->toggle_state = !this->toggle_state;
                }
                return toggle_state;
            } break;
            case hold:
                if (key <= 0) {
                    return false;
                }
                if (GetAsyncKeyState(key)) {
                    return true;
                }
                break;
        }
    }
private:
    bool toggle_state = false;
};
