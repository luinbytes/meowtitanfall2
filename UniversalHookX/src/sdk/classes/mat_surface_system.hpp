#include <cstdint>

class c_mat_surface_system {
private:
    template <unsigned int idx, typename TRet, typename... TArgs>
    static auto call_virtual(void* ThisPtr, TArgs... ArgList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, decltype(ArgList)...);
        return (*static_cast<Fn**>(ThisPtr))[idx](ThisPtr, ArgList...);
    }
public:
    uintptr_t get_embedded_panel() {
        return call_virtual<9, uintptr_t>(this);
    }

    void push_make_current(uintptr_t panel, bool use_in_sets) {
        return call_virtual<11, void>(this, panel, use_in_sets);
    }

    void pop_make_current(uintptr_t panel) {
        return call_virtual<12, void>(this, panel);
    }
};
