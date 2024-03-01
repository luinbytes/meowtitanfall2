
class c_input {
private:
    template <unsigned int idx, typename TRet, typename... TArgs>
    static auto call_virtual(void* ThisPtr, TArgs... ArgList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, decltype(ArgList)...);
        return (*static_cast<Fn**>(ThisPtr))[idx](ThisPtr, ArgList...);
    }

public:
    c_user_cmd* get_user_cmd(int slot, int sequence_number) {
        return call_virtual<9, c_user_cmd*>(this, slot, sequence_number);
    }
};
