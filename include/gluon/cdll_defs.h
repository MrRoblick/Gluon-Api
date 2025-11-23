// --- START OF FILE cdll_defs.h ---
#pragma once
#include <Windows.h>
#include <types.h>

// Стадии кадра в Source Engine
enum ClientFrameStage_t {
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START, // <-- Здесь лучше всего исполнять логику перед отрисовкой
    FRAME_RENDER_END
};

// Определяем указатель на функцию для хука
// thisptr передается первым аргументом в __fastcall
typedef void(__fastcall* FrameStageNotify_proc)(void* thisptr, ClientFrameStage_t stage);

// Получаем доступ к client.dll и функции фабрики интерфейсов
static const auto client_dll = GetModuleHandleA("client.dll");
static const auto clientCreateInterface = reinterpret_cast<CreateInterface_proc>(GetProcAddress(client_dll, "CreateInterface"));