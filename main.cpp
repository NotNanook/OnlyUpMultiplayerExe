#include <iostream>
#include <filesystem>
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
//Library needed by Linker to check file existance
#pragma comment(lib, "Shlwapi.lib")

DWORD GetProcessByName(const char* lpProcessName)
{
    char lpCurrentProcessName[255];

    PROCESSENTRY32 ProcList{};
    ProcList.dwSize = sizeof(ProcList);

    const HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcList == INVALID_HANDLE_VALUE)
        return -1;

    if (!Process32First(hProcList, &ProcList))
        return -1;

    wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

    if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
        return ProcList.th32ProcessID;

    while (Process32Next(hProcList, &ProcList))
    {
        wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

        if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
            return ProcList.th32ProcessID;
    }

    return -1;
}

bool IsProcessRunning(const std::string& pName)
{
    DWORD processes[1024];
    DWORD cbNeeded;
    DWORD cProcesses;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded))
        return false;

    cProcesses = cbNeeded / sizeof(DWORD);

    for (DWORD i = 0; i < cProcesses; i++)
    {
        if (processes[i] == 0)
            continue;

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
        if (hProcess == NULL)
            continue;

        char buffer[MAX_PATH];
        GetModuleBaseNameA(hProcess, NULL, buffer, MAX_PATH);
        CloseHandle(hProcess);

        if (pName == buffer)
            return true;
    }

    return false;
}

VOID RunExecutable(LPCTSTR lpApplicationName)
{
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcess(lpApplicationName,   // the path
        NULL,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void InjectDLL(const int procID,  const char* DllPath)
{
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); // Opening the Process with All Access

    // Allocate memory for the dllpath in the target process, length of the path string + null terminator
    LPVOID pDllPath = VirtualAllocEx(handle, 0, strlen(DllPath) + 1, MEM_COMMIT, PAGE_READWRITE);

    // Write the path to the address of the memory we just allocated in the target process
    WriteProcessMemory(handle, pDllPath, (LPVOID)DllPath, strlen(DllPath) + 1, 0);

    // Create a Remote Thread in the target process which calls LoadLibraryA as our dllpath as an argument -> program loads our dll
    HANDLE hLoadThread = CreateRemoteThread(handle, 0, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), pDllPath, 0, 0);

    WaitForSingleObject(hLoadThread, INFINITE); // Wait for the execution of our loader thread to finish

    VirtualFreeEx(handle, pDllPath, strlen(DllPath) + 1, MEM_RELEASE); // Free the memory allocated for our dll path

    printf("[+] Injected DLL");
}

int main() {
    if (IsProcessRunning("OnlyUP-Win64-Shipping.exe")) {
        printf("[+] Found running OnlyUp process...\n");
    }
    else {
        printf("[i] Didnt found running OnlyUp process...\n");
        printf("[i] Starting OnlyUp...\n");
        LPCWSTR pathToExe = L".\\OnlyUP\\Binaries\\Win64\\OnlyUP-Win64-Shipping.exe";
        ShellExecuteW(NULL, L"open", pathToExe, NULL, NULL, SW_SHOWNORMAL);
        printf("[+] Started OnlyUp...\n");
    }

    Sleep(5000);
    const DWORD pid = GetProcessByName("OnlyUP-Win64-Shipping.exe");
    while (pid == (DWORD)-1) {
        const DWORD pid = GetProcessByName("OnlyUP-Win64-Shipping.exe");
    }

    if (PathFileExists(L"OnlyUpMultiplayer.dll") == FALSE)
    {
        printf("[!] DLL file does NOT exist\n");
        Sleep(10000);
        return EXIT_FAILURE;
    }

    printf("[+] Located DLL and Process\n");

    printf("[i] Injecting...\n");
    std::filesystem::path path = std::filesystem::current_path();
    path.append("OnlyUpMultiplayer.dll");
    std::string pathToDLL = path.u8string();
    InjectDLL(pid, pathToDLL.c_str());

    Sleep(10000);
}