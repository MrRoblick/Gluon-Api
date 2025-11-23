#include <Gluon.h>
#include <iostream>
#include <Windows.h>
#include <CLuaShared.h>
#include <lua_defs.h>
#include <vgui2_defs.h>
#include <d3d9_defs.h>
#include <safetyhook.hpp>
#include <mutex>
#include <thread>
#include <queue>
#include <Panel.h>

static CLuaShared* ILuaInterface = nullptr;
static IPanel* g_pPanel = nullptr;
static SafetyHookInline g_paint_traverse_hook{};
static SafetyHookInline g_end_scene_hook{};

static std::queue<std::string> scripts;
static std::mutex scriptMtx;
static unsigned int g_MatSystemTopPanel = 0;

static inline void initConsole() {
    if (!AllocConsole()) return;
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    std::cout.clear();
}

static void* GetD3D9EndSceneAddress()
{
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D)
        return nullptr;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, NULL, NULL);

    if (!d3dpp.hDeviceWindow)
    {
        pD3D->Release();
        return nullptr;
    }

    IDirect3DDevice9* pDevice = nullptr;

    HRESULT hr = pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        d3dpp.hDeviceWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDevice
    );

    if (FAILED(hr))
    {
        hr = pD3D->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_REF,
            d3dpp.hDeviceWindow,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &d3dpp,
            &pDevice
        );
    }

    void* endSceneAddress = nullptr;

    if (SUCCEEDED(hr) && pDevice)
    {
        void** vTable = *reinterpret_cast<void***>(pDevice);

        endSceneAddress = vTable[42];

        pDevice->Release();
    }

    pD3D->Release();
    DestroyWindow(d3dpp.hDeviceWindow);

    return endSceneAddress;
}


static void mainThread() {
    if (!ILuaInterface) return;

    auto clientInterface = ILuaInterface->GetLuaInterface(LuaInterfaceType::CLIENT);
    if (!clientInterface) return;

    auto const luaState = clientInterface->state;
    if (!luaState) return;

    std::queue<std::string> localScripts;
    {
        std::lock_guard<std::mutex> lock(scriptMtx);
        if (scripts.empty()) return;
        localScripts.swap(scripts);
    }

    while (!localScripts.empty()) {
        auto& source = localScripts.front();
        if (luaL_loadstring(luaState, source.c_str()) == 0) {
            if (lua_pcall(luaState, 0, 0, 0) != 0) {
                const char* err = lua_tostring(luaState, -1);
                std::cout << "[Lua Error] " << (err ? err : "Unknown") << std::endl;
                lua_pop(luaState, 1);
            }
        }
        else {
            const char* err = lua_tostring(luaState, -1);
            std::cout << "[Lua Compile Error] " << (err ? err : "Unknown") << std::endl;
            lua_pop(luaState, 1);
        }
        localScripts.pop();
    }
}

// TEST
void hkPaintTraverse(void* thisptr, unsigned int vguiPanel, bool forceRepaint, bool allowForce){
    g_paint_traverse_hook.stdcall<void>(thisptr, vguiPanel, forceRepaint, allowForce); // <-- Broken
}

static HRESULT* APIENTRY hkEndScene(IDirect3DDevice9* pDevice) {
    mainThread();
    return g_end_scene_hook.call<HRESULT*>(pDevice);
}


void Gluon::init(bool debug) {
    if (debug) initConsole();

    const auto EndScene = GetD3D9EndSceneAddress();

    ILuaInterface = reinterpret_cast<CLuaShared*>(LuaSharedCreateInterface("LUASHARED003", nullptr));
    if (!ILuaInterface) throw std::runtime_error("Failed to get LUASHARED003");

    /*
    g_pPanel = reinterpret_cast<IPanel*>(vgui2CreateInterface("VGUI_Panel009", nullptr));
    if (!g_pPanel) throw std::runtime_error("Failed to get VGUI_Panel009");

    void** vTable = *reinterpret_cast<void***>(g_pPanel);
    void* originalPaintTraverse = vTable[41];

    std::cout << "Hooking PaintTraverse: " << originalPaintTraverse << std::endl;
    */
    //g_paint_traverse_hook = safetyhook::create_inline(originalPaintTraverse, reinterpret_cast<void*>(hkPaintTraverse));
    g_end_scene_hook = safetyhook::create_inline(EndScene, reinterpret_cast<void*>(hkEndScene));

}

void Gluon::execute(std::string script) {
    std::lock_guard<std::mutex> lock(scriptMtx);
    scripts.push(std::move(script));
}