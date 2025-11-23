#pragma once
#include <Windows.h>
#include <types.h>

static const auto vgui2_dll = GetModuleHandleA("vgui2.dll");
static const auto vgui2CreateInterface = reinterpret_cast<CreateInterface_proc>(GetProcAddress(vgui2_dll, "CreateInterface"));