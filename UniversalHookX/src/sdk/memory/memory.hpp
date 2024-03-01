#include <chrono>

#include <windows.h>

#include "lazy_importer.hpp"

enum e_modules {
    game_base,
    client,
    engine,
    mat_system_surface,
    vgui,
    input_system,

    max
};

#define RVA(instr, instr_size) ((uintptr_t)instr + instr_size + *(long*)((uintptr_t)instr + (instr_size - sizeof(long))))

namespace memory {
    void init();

    uintptr_t get_pattern(uintptr_t base, const char* signature);

    inline uintptr_t base_game;
    inline uintptr_t client;
    inline uintptr_t engine;
    inline uintptr_t vguimatsurface;
    inline uintptr_t vgui2;
    inline uintptr_t inputsystem;

    template <unsigned int idx, typename TRet, typename... TArgs>
    static auto call_virtual(void* ThisPtr, TArgs... ArgList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, decltype(ArgList)...);
        return (*static_cast<Fn**>(ThisPtr))[idx](ThisPtr, ArgList...);
    }
}
