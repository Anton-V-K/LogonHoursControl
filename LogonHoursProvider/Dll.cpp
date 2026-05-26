#include "stdafx.h"

#include "ClassFactory.h"
#include "Dll.h"
#include "Guids.h"

static HINSTANCE g_hInstance = nullptr;
static long g_dllRefCount = 0;

void DllAddRef()
{
    InterlockedIncrement(&g_dllRefCount);
}

void DllRelease()
{
    InterlockedDecrement(&g_dllRefCount);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        g_hInstance = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
    }
    return TRUE;
}

STDAPI DllCanUnloadNow()
{
    return g_dllRefCount == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (!ppv)
        return E_INVALIDARG;
    *ppv = nullptr;
    if (rclsid != CLSID_LogonHoursMonitorProvider)
        return CLASS_E_CLASSNOTAVAILABLE;

    auto* factory = new (std::nothrow) ClassFactory();
    if (!factory)
        return E_OUTOFMEMORY;

    const HRESULT hr = factory->QueryInterface(riid, ppv);
    factory->Release();
    return hr;
}

STDAPI DllRegisterServer()
{
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileName(g_hInstance, modulePath, _countof(modulePath)) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    const wchar_t* description = L"Logon Hours Monitor";
    const wchar_t* providerKey =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\"
        L"{8F2E4A9D-1B3C-4E5F-9A6B-7C8D9E0F1A2B}";
    const wchar_t* clsidKey = L"CLSID\\{8F2E4A9D-1B3C-4E5F-9A6B-7C8D9E0F1A2B}";
    const wchar_t* inprocKey =
        L"CLSID\\{8F2E4A9D-1B3C-4E5F-9A6B-7C8D9E0F1A2B}\\InprocServer32";

    HKEY key = nullptr;
    LSTATUS status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, providerKey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr);
    if (status != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(status);
    RegSetValueEx(key, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(description),
        static_cast<DWORD>((wcslen(description) + 1) * sizeof(wchar_t)));
    RegCloseKey(key);

    status = RegCreateKeyEx(HKEY_CLASSES_ROOT, clsidKey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr);
    if (status != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(status);
    RegSetValueEx(key, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(description),
        static_cast<DWORD>((wcslen(description) + 1) * sizeof(wchar_t)));
    RegCloseKey(key);

    status = RegCreateKeyEx(HKEY_CLASSES_ROOT, inprocKey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr);
    if (status != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(status);
    RegSetValueEx(key, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(modulePath),
        static_cast<DWORD>((wcslen(modulePath) + 1) * sizeof(wchar_t)));
    const wchar_t* apartment = L"Apartment";
    RegSetValueEx(key, L"ThreadingModel", 0, REG_SZ, reinterpret_cast<const BYTE*>(apartment),
        static_cast<DWORD>((wcslen(apartment) + 1) * sizeof(wchar_t)));
    RegCloseKey(key);

    return S_OK;
}

STDAPI DllUnregisterServer()
{
    const wchar_t* providerKey =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\"
        L"{8F2E4A9D-1B3C-4E5F-9A6B-7C8D9E0F1A2B}";
    const wchar_t* clsidKey = L"CLSID\\{8F2E4A9D-1B3C-4E5F-9A6B-7C8D9E0F1A2B}";

    RegDeleteTree(HKEY_LOCAL_MACHINE, providerKey);
    RegDeleteTree(HKEY_CLASSES_ROOT, clsidKey);
    return S_OK;
}
