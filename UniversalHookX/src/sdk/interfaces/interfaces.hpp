#include <chrono>
#include "../classes/enums.hpp"
#include "../classes/mat_surface_system.hpp"
#include "../classes/user_cmd.hpp"
#include "../classes/input.hpp"

namespace interfaces {
    void init( );

    inline c_mat_surface_system* mat_surface_system = nullptr;
    inline c_input* input = nullptr;
}
