#include "stdafx.h"

#include "UserAccounts.h"

#include <LM.h>

#pragma comment(lib, "Netapi32.lib")

std::vector<std::wstring> FetchLocalUserAccounts()
{
    std::vector<std::wstring> users;

    LPBYTE buffer = nullptr;
    DWORD entriesread = 0;
    DWORD totalentries = 0;
    if (NetUserEnum(NULL, 1, FILTER_NORMAL_ACCOUNT, &buffer, MAX_PREFERRED_LENGTH, &entriesread, &totalentries, NULL) != NERR_Success)
        return users;

    const auto* const userinfo = reinterpret_cast<const USER_INFO_1*>(buffer);
    for (DWORD i = 0; i < entriesread; ++i)
    {
        if ((userinfo[i].usri1_flags & USER_PRIV_MASK) == USER_PRIV_USER)
            users.push_back(userinfo[i].usri1_name);
    }
    NetApiBufferFree(buffer);
    return users;
}

bool LoadLogonHoursForUsers(std::map<std::wstring, LogonHours>& hours)
{
    hours.clear();
    const auto users = FetchLocalUserAccounts();
    for (const auto& user : users)
    {
        LogonHours lh;
        if (!lh.InitFrom(user.c_str()))
            return false;
        hours[user] = lh;
    }
    return true;
}
