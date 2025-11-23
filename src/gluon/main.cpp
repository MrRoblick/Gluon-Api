#include <iostream>

#define WIN32_LEAN_AND_MEAN 
#include <crow_all.h>
#include <Windows.h>

#include <Gluon.h>
#include <thread>
#include <GluonPacket.h>

static void InitWebServer() {
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/")([]() {
        return "Bro, what you expected? :-)";
        });

    CROW_ROUTE(app, "/execute")
        .methods(crow::HTTPMethod::Post)
        ([](const crow::request& req) {
        Gluon::execute(req.body);

        return crow::response(200);
            });

    std::cout << "Listening localhost:8887" << std::endl;

    app.port(8887).bindaddr("127.0.0.1").multithreaded().run();
}

int WINAPI MainThread(void* p) {
    Gluon::init(false);
    std::thread(InitWebServer).detach();
    return 0;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hinstDLL);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
        break;
    }

    case DLL_THREAD_ATTACH: {
        break;
    }

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
    {
        break;
    }
    }
    return TRUE;
}