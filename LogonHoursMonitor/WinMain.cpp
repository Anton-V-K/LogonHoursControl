#include "stdafx.h"

#include "TrayWnd.h"

CAppModule _Module;

namespace
{
    bool EnsureSingleInstance()
    {
        HANDLE mutex = CreateMutex(nullptr, TRUE, L"LogonHoursMonitor");
        if (!mutex)
            return false;
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(mutex);
            return false;
        }
        return true;
    }

    void EnsureAutostart()
    {
        wchar_t exePath[MAX_PATH];
        if (GetModuleFileName(nullptr, exePath, _countof(exePath)) == 0)
            return;

        HKEY key = nullptr;
        if (RegOpenKeyEx(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, KEY_SET_VALUE | KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
            return;

        wchar_t existing[MAX_PATH];
        DWORD size = sizeof(existing);
        const LSTATUS query = RegQueryValueEx(key, L"LogonHoursMonitor", nullptr, nullptr, reinterpret_cast<LPBYTE>(existing), &size);
        if (query != ERROR_SUCCESS || _wcsicmp(existing, exePath) != 0)
            RegSetValueEx(key, L"LogonHoursMonitor", 0, REG_SZ, reinterpret_cast<const BYTE*>(exePath),
                static_cast<DWORD>((wcslen(exePath) + 1) * sizeof(wchar_t)));

        RegCloseKey(key);
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
    LogMain("LogonHoursMonitor");

    if (!EnsureSingleInstance())
        return 0;

    _Module.Init(nullptr, hInstance);
    EnsureAutostart();

    CTrayWnd tray;
    if (!tray.Create())
    {
        _Module.Term();
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    _Module.Term();
    return static_cast<int>(msg.wParam);
}
