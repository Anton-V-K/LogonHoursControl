#include "stdafx.h"

#include <Common/LogonHoursText.h>
#include <Common/UserAccounts.h>

#include "TrayWnd.h"

BOOL CTrayWnd::Create()
{
    HWND hwnd = CWindowImpl<CTrayWnd>::Create(HWND_MESSAGE, nullptr, L"LogonHoursMonitor", WS_POPUP);
    if (!hwnd)
        return FALSE;

    AddTrayIcon();
    return TRUE;
}

void CTrayWnd::AddTrayIcon()
{
    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = m_hWnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon = ::LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_APP));
    wcscpy_s(m_nid.szTip, L"Logon Hours Monitor");
    Shell_NotifyIcon(NIM_ADD, &m_nid);
    m_trayAdded = true;
}

void CTrayWnd::RemoveTrayIcon()
{
    if (m_trayAdded)
    {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
        m_trayAdded = false;
    }
    if (m_nid.hIcon)
    {
        DestroyIcon(m_nid.hIcon);
        m_nid.hIcon = nullptr;
    }
}

LRESULT CTrayWnd::OnTrayIcon(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
    if (wParam != m_nid.uID)
        return 0;

    switch (LOWORD(lParam))
    {
    case WM_RBUTTONUP:
    case WM_CONTEXTMENU:
        ShowContextMenu();
        break;
    case WM_LBUTTONDBLCLK:
        ShowHoursSummary();
        break;
    }
    return 0;
}

void CTrayWnd::ShowContextMenu()
{
    HMENU menu = ::CreatePopupMenu();
    if (!menu)
        return;

    AppendMenu(menu, MF_STRING, IDM_TRAY_SHOW_HOURS, L"Show allowed logon hours...");
    AppendMenu(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(menu, MF_STRING, IDM_TRAY_EXIT, L"E&xit");

    POINT pt;
    GetCursorPos(&pt);
    ::SetForegroundWindow(m_hWnd);
    const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, nullptr);
    DestroyMenu(menu);

    if (cmd == IDM_TRAY_EXIT)
        PostMessage(WM_CLOSE);
    else if (cmd == IDM_TRAY_SHOW_HOURS)
        ShowHoursSummary();
}

void CTrayWnd::ShowHoursSummary()
{
    std::map<std::wstring, LogonHours> hours;
    if (!LoadLogonHoursForUsers(hours))
    {
        MessageBox(L"Failed to load logon hours for local users.", L"Logon Hours Monitor", MB_ICONERROR);
        return;
    }

    const std::wstring text = FormatAllUsersLogonHours(hours);
    MessageBox(text.c_str(), L"Allowed logon hours (local time)", MB_OK | MB_ICONINFORMATION);
}

LRESULT CTrayWnd::OnClose(UINT, WPARAM, LPARAM, BOOL&)
{
    DestroyWindow();
    return 0;
}

LRESULT CTrayWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL&)
{
    RemoveTrayIcon();
    PostQuitMessage(0);
    return 0;
}
