#pragma once

#include <map>
#include <string>
#include <vector>

#include "LogonHours.h"

std::vector<std::wstring> FetchLocalUserAccounts();

bool LoadLogonHoursForUsers(std::map<std::wstring, LogonHours>& hours);
