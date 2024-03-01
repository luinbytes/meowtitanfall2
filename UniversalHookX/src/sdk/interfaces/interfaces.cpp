#include "../include.hpp"

typedef void* (*create_interface_fn)(const char* name, int* return_code);
void* create_interface(const char* module_name, const char* interface_name) {
    const auto base_address = LI_FN(GetProcAddress)(LI_FN(GetModuleHandleA)(module_name), "CreateInterface");
    const auto create_interface = reinterpret_cast<create_interface_fn>(base_address);

    auto ret = create_interface(interface_name, nullptr);

    return ret;
}

void interfaces::init( ) {
    mat_surface_system = reinterpret_cast<c_mat_surface_system*>(create_interface("vguimatsurface.dll", "VGUI_Surface031"));
    input = reinterpret_cast<c_input*>(RVA(memory::get_pattern(memory::client, "48 8B 05 ? ? ? ? 48 8D 0D ? ? ? ? FF 50 50 83 BB"), 7));
}
