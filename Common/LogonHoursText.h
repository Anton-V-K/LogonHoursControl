#pragma once

#include <map>
#include <string>

#include "LogonHours.h"

// Human-readable summary (local time), similar to `net user USER /time:...`.
std::wstring FormatLogonHours(const LogonHours& hours);

// Multi-line text: one line per local user account.
std::wstring FormatAllUsersLogonHours(const std::map<std::wstring, LogonHours>& users);
