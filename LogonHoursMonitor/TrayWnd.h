#pragma once

#include "Resource.h"

class CTrayWnd : public CWindowImpl<CTrayWnd>
{
public:
    DECLARE_WND_CLASS(L"LogonHoursMonitorTrayWnd")

    enum { WM_TRAYICON = WM_APP + 1 };

    BOOL Create();
    void RemoveTrayIcon();

    BEGIN_MSG_MAP(CTrayWnd)
        MESSAGE_HANDLER(WM_TRAYICON, OnTrayIcon)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

    LRESULT OnTrayIcon(UINT, WPARAM wParam, LPARAM lParam, BOOL&);
    LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);

private:
    void AddTrayIcon();
    void ShowContextMenu();
    void ShowHoursSummary();

    NOTIFYICONDATA m_nid{};
    bool m_trayAdded{ false };
};
