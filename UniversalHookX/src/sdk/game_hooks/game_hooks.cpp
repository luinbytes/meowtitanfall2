#include "../../dependencies/minhook/MinHook.h"
#include "../include.hpp"
#include "../../features/settings.hpp"

extern bool __fastcall create_move(__int64 ptr, float a2, c_user_cmd* cmd, unsigned __int8 a4);
static decltype(&create_move) create_move_original;
bool __fastcall create_move(__int64 ptr, float a2, c_user_cmd* cmd, unsigned __int8 a4) {

    if (!cmd->command_number)
        return create_move_original(ptr, a2, cmd, a4);

    if ( settings::exploits::speed_hack && settings::exploits::speed_hack_key.get_state())
        *(float*)((uintptr_t)cmd + 0xAC) += 0.025f;
    return create_move_original(ptr, a2, cmd, a4);
}

extern bool __fastcall screenshot_write_to_buffer(__int64 a1, __int64 a2);
static decltype(&screenshot_write_to_buffer) screenshot_write_to_buffer_original;
bool __fastcall screenshot_write_to_buffer(__int64 a1, __int64 a2) {

    printf("[!] CLC_Screenshot::WriteToBuffer was called!\n");
    return false;
}

extern bool __fastcall screenshot_read_from_buffer(__int64 a1, __int64 a2);
static decltype(&screenshot_read_from_buffer) screenshot_read_from_buffer_original;
bool __fastcall screenshot_read_from_buffer(__int64 a1, __int64 a2) {

    printf("[!] CLC_Screenshot::ReadFromBuffer was called!\n");
    return false;
}

void game_hooks::init( ) {
    void* screenshot_write_to_buffer_target = (void*)memory::get_pattern(memory::engine, "48 89 5C 24 ? 57 48 83 EC 20 8B 42 10 48 8B DA 48 8B F9 25 ? ? ? ? 7D 07 FF C8 83 C8");
    if (MH_CreateHook(screenshot_write_to_buffer_target, &screenshot_write_to_buffer, (void**)&screenshot_write_to_buffer_original) == MH_OK) {
        MH_EnableHook(screenshot_write_to_buffer_target);
        printf("[+] screenshot_write_to_buffer_target hooked!\n");
    } else {
        printf("[!] screenshot_write_to_buffer_target failed hooked!\n");
    }

    void* screenshot_read_from_buffer_target = (void*)memory::get_pattern(memory::engine, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B DA 48 8B 52");
    if (MH_CreateHook(screenshot_read_from_buffer_target, &screenshot_read_from_buffer, (void**)&screenshot_read_from_buffer_original) == MH_OK) {
        MH_EnableHook(screenshot_read_from_buffer_target);
        printf("[+] screenshot_read_from_buffer_target hooked!\n");
    } else {
        printf("[!] screenshot_read_from_buffer_target failed hooked!\n");
    }

    void* create_move_target = (void*)memory::get_pattern(memory::client, "48 89 5C 24 ? 57 48 83 EC 30 48 8B 0D ? ? ? ? 0F 29 74 24 ? 41 0F");
    if (MH_CreateHook(create_move_target, &create_move, (void**)&create_move_original) == MH_OK) {
        MH_EnableHook(create_move_target);
        printf("[+] create_move hooked!\n");
    } else {
        printf("[!] create_move failed hooked!\n");
    }
}
