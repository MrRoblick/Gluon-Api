#include <iostream>
#include <injector.h>
#include <optional>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

std::optional<PROCESSENTRY32> FindProcess(std::string processName) {
	HANDLE hwnd = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 proc{};
	proc.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hwnd, &proc);
	do {
		if (processName == proc.szExeFile) {
			CloseHandle(hwnd);
			return std::optional<PROCESSENTRY32>{proc};
		}
	} while (Process32Next(hwnd, &proc));
	CloseHandle(hwnd);
	return std::optional<PROCESSENTRY32>{};
}

int main(int argc, char* argv[]) {
	std::cout << argc << std::endl;
	if (argc < 2) {
		std::cerr << "Process name is null" << std::endl;
		return 7;
	}
	if (argc < 3) {
		std::cerr << "Dll name is null" << std::endl;
		return 8;
	}
	const std::string processName = std::string(argv[1]);
	const char* const dllPath = argv[2];

	std::optional<PROCESSENTRY32> proc = FindProcess(processName);
	if (!proc.has_value()) {
		std::cerr << "Failed to find process " << processName << std::endl;
		return 1;
	}
	DWORD processId = proc->th32ProcessID;

	TOKEN_PRIVILEGES priv{};
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid)) {
			AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);
		}
		CloseHandle(hToken);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (!hProc) {
		DWORD err = GetLastError();
		std::cout << "OpenProcess failed 0x" << std::hex << err << std::endl;
		system("PAUSE");
		return 1;
	}
	if (GetFileAttributes(dllPath) == INVALID_FILE_ATTRIBUTES) {
		std::cout << "Dll file doesn't exist\n";
		CloseHandle(hProc);
		system("PAUSE");
		return 2;
	}

	std::ifstream File{ dllPath, std::ios::binary | std::ios::ate };
	if (File.fail()) {
		std::cout << "Opening the file failed: " << std::hex << static_cast<DWORD>(File.rdstate()) << std::endl;
		File.close();
		CloseHandle(hProc);
		system("PAUSE");
		return 3;
	}

	auto FileSize = File.tellg();
	if (FileSize < 0x1000) {
		std::cout << "Filesize invalid.\n";
		File.close();
		CloseHandle(hProc);
		system("PAUSE");
		return 4;
	}

	BYTE* pSrcData = new BYTE[static_cast<UINT_PTR>(FileSize)];
	if (!pSrcData) {
		std::cout << "Can't allocate dll file.\n";
		File.close();
		CloseHandle(hProc);
		system("PAUSE");
		return 5;
	}

	File.seekg(0, std::ios::beg);
	File.read(reinterpret_cast<char*>(pSrcData), FileSize);

	std::cout << "Mapping...\n";
	if (!ManualMapDll(hProc, pSrcData, FileSize)) {
		delete[] pSrcData;
		CloseHandle(hProc);
		std::cout << "Error while mapping.\n";
		system("PAUSE");
		return 6;
	}
	delete[] pSrcData;

	CloseHandle(hProc);
	std::cout << "Successfully injected\n";
	return 0;
}