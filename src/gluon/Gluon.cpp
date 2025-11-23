#include <Gluon.h>
#include <iostream>
#include <Windows.h>
#include <CLuaShared.h>
#include <lua_defs.h>
#include <d3d9_defs.h>
#include <safetyhook.hpp>
#include <mutex>
#include <thread>
#include <queue>
#include <exception>
#include <string>

static SafetyHookInline g_endscene_hook{};
static CLuaShared* ILuaInterface = nullptr;
static EndScene_proc EndScene = nullptr;

static std::queue<std::string> scripts;
static std::mutex scriptMtx;

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

static inline void initConsole() {
    if (!AllocConsole()) {
        return;
    }

    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();
}

static void mainThread() {
    std::queue<std::string> localScripts;

    {
        std::lock_guard<std::mutex> lock(scriptMtx);
        if (scripts.empty()) return;

        localScripts.swap(scripts);
    }

    auto const clientInterface = ILuaInterface->GetLuaInterface(LuaInterfaceType::CLIENT);
    if (!clientInterface) return;
    auto const luaState = clientInterface->state;
    if (!luaState) return;

    while (!localScripts.empty()) {
        auto& source = localScripts.front();
        if (!luaL_loadstring(luaState, source.c_str())) {
            lua_pcall(luaState, 0, 0, 0);
        }
        localScripts.pop();
    }
}

static HRESULT* APIENTRY EndScene_hook(IDirect3DDevice9* pDevice) {
    mainThread();
    return g_endscene_hook.call<HRESULT*>(pDevice);
}

void Gluon::init(bool debug) {
    if(debug) initConsole();
    const auto addy = GetD3D9EndSceneAddress();
    std::cout << "EndScene address: " << addy << std::endl;
    if (!addy) throw std::runtime_error("Failed to get DirectX9 EndScene function address");
    EndScene = reinterpret_cast<EndScene_proc>(addy);
    ILuaInterface = reinterpret_cast<CLuaShared*>(CreateInterface("LUASHARED003", nullptr));
    std::cout << "ILuaInterface: " << ILuaInterface << std::endl;
    if (!addy) throw std::runtime_error("Failed to get ILuaInterface 'LUASHARED003'");
    g_endscene_hook = safetyhook::create_inline(reinterpret_cast<void*>(EndScene), reinterpret_cast<void*>(EndScene_hook));
}

void Gluon::execute(std::string script) {
    scripts.push(std::move(script));
}