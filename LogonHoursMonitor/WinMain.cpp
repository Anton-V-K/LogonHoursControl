#include "stdafx.h"

#include "CpInstall.h"
#include "TrayWnd.h"

CAppModule _Module;

namespace
{
    bool HasCommandLineSwitch(const wchar_t* switchName)
    {
        int argc = 0;
        wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (!argv)
            return false;

        bool found = false;
        for (int i = 1; i < argc; ++i)
        {
            if (_wcsicmp(argv[i], switchName) == 0)
            {
                found = true;
                break;
            }
        }
        LocalFree(argv);
        return found;
    }

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

    if (HasCommandLineSwitch(L"--install-cp"))
    {
        if (!InstallCredentialProvider())
        {
            MessageBox(nullptr, L"Failed to register the sign-in credential provider.\nRun as Administrator.",
                L"Logon Hours Monitor", MB_ICONERROR);
            return 1;
        }
        MessageBox(nullptr, L"Sign-in credential provider registered.", L"Logon Hours Monitor", MB_OK);
        return 0;
    }

    if (HasCommandLineSwitch(L"--uninstall-cp"))
    {
        if (!UninstallCredentialProvider())
        {
            MessageBox(nullptr, L"Failed to unregister the sign-in credential provider.\nRun as Administrator.",
                L"Logon Hours Monitor", MB_ICONERROR);
            return 1;
        }
        MessageBox(nullptr, L"Sign-in credential provider unregistered.", L"Logon Hours Monitor", MB_OK);
        return 0;
    }

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
