#pragma once

template <typename T>
inline T get_vfunc(void* instance, size_t index) {
    return (*reinterpret_cast<T**>(instance))[index];
}

class IPanel {
public:
    const char* GetName(unsigned int vguiPanel) {
        typedef const char* (__fastcall* GetNameFn)(void* thisptr, unsigned int vguiPanel);
        return get_vfunc<GetNameFn>(this, 36)(this, vguiPanel);
    }
};

typedef void(*PaintTraverse_proc)(void* thisptr, unsigned int vguiPanel, bool forceRepaint, bool allowForce);