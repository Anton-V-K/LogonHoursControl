#include "stdafx.h"

#include "CpInstall.h"

#include <Common/Logger.h>

namespace
{
    std::wstring GetCredentialProviderDllPath()
    {
        wchar_t exePath[MAX_PATH];
        const DWORD len = GetModuleFileName(nullptr, exePath, _countof(exePath));
        if (len == 0 || len >= _countof(exePath))
            return {};

        wchar_t* slash = wcsrchr(exePath, L'\\');
        if (!slash)
            return {};

        wcscpy_s(slash + 1, _countof(exePath) - (slash - exePath + 1), L"LogonHoursProvider.dll");
        return exePath;
    }

    bool IsPe64BitDll(const wchar_t* dllPath)
    {
        HANDLE file = CreateFile(dllPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (file == INVALID_HANDLE_VALUE)
            return false;

        IMAGE_DOS_HEADER dos{};
        DWORD read = 0;
        if (!ReadFile(file, &dos, sizeof(dos), &read, nullptr) || dos.e_magic != IMAGE_DOS_SIGNATURE)
        {
            CloseHandle(file);
            return false;
        }

        SetFilePointer(file, dos.e_lfanew, nullptr, FILE_BEGIN);
        DWORD signature = 0;
        if (!ReadFile(file, &signature, sizeof(signature), &read, nullptr) || signature != IMAGE_NT_SIGNATURE)
        {
            CloseHandle(file);
            return false;
        }

        IMAGE_FILE_HEADER fileHeader{};
        if (!ReadFile(file, &fileHeader, sizeof(fileHeader), &read, nullptr))
        {
            CloseHandle(file);
            return false;
        }

        CloseHandle(file);
        return fileHeader.Machine == IMAGE_FILE_MACHINE_AMD64;
    }

    std::wstring GetRegsvr32Path(const wchar_t* dllPath)
    {
        wchar_t systemDir[MAX_PATH];
        if (!GetSystemDirectory(systemDir, _countof(systemDir)))
            return {};

        const bool dllIs64 = IsPe64BitDll(dllPath);
#if defined(_WIN64)
        const bool use64 = dllIs64;
#else
        const bool use64 = dllIs64;
#endif
        UNREFERENCED_PARAMETER(use64);

        if (dllIs64)
        {
#if defined(_WIN64)
            return std::wstring(systemDir) + L"\\regsvr32.exe";
#else
            wchar_t windowsDir[MAX_PATH];
            if (!GetWindowsDirectory(windowsDir, _countof(windowsDir)))
                return {};
            return std::wstring(windowsDir) + L"\\Sysnative\\regsvr32.exe";
#endif
        }

#if defined(_WIN64)
        wchar_t windowsDir[MAX_PATH];
        if (!GetWindowsDirectory(windowsDir, _countof(windowsDir)))
            return {};
        return std::wstring(windowsDir) + L"\\SysWOW64\\regsvr32.exe";
#else
        return std::wstring(systemDir) + L"\\regsvr32.exe";
#endif
    }

    bool RunRegsvr32(const wchar_t* dllPath, bool uninstall)
    {
        const std::wstring regsvr32 = GetRegsvr32Path(dllPath);
        if (regsvr32.empty())
            return false;

        wchar_t cmdLine[MAX_PATH * 3];
        if (uninstall)
            swprintf_s(cmdLine, L"\"%s\" /s /u \"%s\"", regsvr32.c_str(), dllPath);
        else
            swprintf_s(cmdLine, L"\"%s\" /s \"%s\"", regsvr32.c_str(), dllPath);

        STARTUPINFO si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        wchar_t mutableCmd[MAX_PATH * 3];
        wcscpy_s(mutableCmd, cmdLine);
        if (!CreateProcess(nullptr, mutableCmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
        {
            LOG_ERROR(__func__) << "CreateProcess(regsvr32) failed with error " << GetLastError();
            return false;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode = 1;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return exitCode == 0;
    }
}

bool IsCredentialProviderRegistered()
{
    HKEY key = nullptr;
    const LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\"
        L"{8F2E4A9D-1B3C-4E5F-9A6B-7C8D9E0F1A2B}",
        0, KEY_READ | KEY_WOW64_64KEY, &key);
    if (status == ERROR_SUCCESS)
        RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

bool InstallCredentialProvider()
{
    const std::wstring dllPath = GetCredentialProviderDllPath();
    if (dllPath.empty() || GetFileAttributes(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        LOG_ERROR(__func__) << "Credential provider DLL not found: " << dllPath.c_str();
        return false;
    }
    return RunRegsvr32(dllPath.c_str(), false);
}

bool UninstallCredentialProvider()
{
    const std::wstring dllPath = GetCredentialProviderDllPath();
    if (dllPath.empty())
        return false;
    return RunRegsvr32(dllPath.c_str(), true);
}
