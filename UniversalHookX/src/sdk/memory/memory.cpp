#include "../include.hpp"

#define IN_RANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x) (IN_RANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA) : (IN_RANGE(x, '0', '9') ? x - '0' : 0))
#define GET_BYTE(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

void memory::init( ) {
    base_game = reinterpret_cast<uintptr_t>(LI_MODULE("Titanfall2.exe").get( ));
    client = reinterpret_cast<uintptr_t>(LI_MODULE("client.dll").get( ));
    engine = reinterpret_cast<uintptr_t>(LI_MODULE("engine.dll").get( ));
    vguimatsurface = reinterpret_cast<uintptr_t>(LI_MODULE("vguimatsurface.dll").get( ));
    vgui2 = reinterpret_cast<uintptr_t>(LI_MODULE("vgui2.dll").get( ));
    inputsystem = reinterpret_cast<uintptr_t>(LI_MODULE("inputsystem.dll").get( ));
}

uintptr_t memory::get_pattern(uintptr_t base, const char* signature) {
    const auto DOSHeader = PIMAGE_DOS_HEADER(base);
    const auto NTHeaders = PIMAGE_NT_HEADERS(reinterpret_cast<std::uint8_t*>(base) + DOSHeader->e_lfanew);
    uintptr_t Size = (base + (uintptr_t)NTHeaders->OptionalHeader.SizeOfImage);

    const char* Pat = signature;
    uintptr_t FirstMatch = 0;
    for (uintptr_t Cur = base; Cur < Size; Cur++) {
        if (!*Pat)
            return FirstMatch;

        if (*(PBYTE)Pat == ('\?') || *(BYTE*)Cur == GET_BYTE(Pat)) {
            if (!FirstMatch)
                FirstMatch = Cur;

            if (!Pat[2])
                return FirstMatch;

            if (*(PWORD)Pat == ('\?\?') || *(PBYTE)Pat != ('\?'))
                Pat += 3;

            else
                Pat += 2;
        } else {
            Pat = signature;
            FirstMatch = 0;
        }
    }

    return NULL;
}
