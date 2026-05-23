#include "stdafx.h"

#include "LogonHoursText.h"

namespace
{
    static const wchar_t* const dayAbbr[] = { L"Su", L"M", L"T", L"W", L"Th", L"F", L"Sa" };

    struct DaySchedule
    {
        bool allowed[24]{};
    };

    void AppendHourRange(std::wstring& out, int startHour, int endHourExclusive)
    {
        if (!out.empty())
            out += L',';
        wchar_t buf[32];
        if (endHourExclusive == startHour + 1)
            swprintf_s(buf, L"%d", startHour);
        else
            swprintf_s(buf, L"%d-%d", startHour, endHourExclusive);
        out += buf;
    }

    std::wstring FormatDaySchedule(const DaySchedule& day)
    {
        std::wstring ranges;
        int runStart = -1;
        for (int hour = 0; hour < 24; ++hour)
        {
            if (day.allowed[hour])
            {
                if (runStart < 0)
                    runStart = hour;
            }
            else if (runStart >= 0)
            {
                AppendHourRange(ranges, runStart, hour);
                runStart = -1;
            }
        }
        if (runStart >= 0)
            AppendHourRange(ranges, runStart, 24);
        return ranges;
    }

    bool SameSchedule(const DaySchedule& a, const DaySchedule& b)
    {
        return memcmp(a.allowed, b.allowed, sizeof(a.allowed)) == 0;
    }

    void AppendDayRange(std::wstring& out, int startDay, int endDay)
    {
        if (!out.empty())
            out += L',';
        if (startDay == endDay)
            out += dayAbbr[startDay];
        else
        {
            wchar_t buf[16];
            swprintf_s(buf, L"%s-%s", dayAbbr[startDay], dayAbbr[endDay]);
            out += buf;
        }
    }

    std::wstring FormatWeekSchedule(const WeekHours& week)
    {
        DaySchedule days[7];
        for (int day = 0; day < 7; ++day)
        {
            for (int hour = 0; hour < 24; ++hour)
                days[day].allowed[hour] = week.table[day][hour];
        }

        std::wstring result;
        for (int day = 0; day < 7; )
        {
            const std::wstring hours = FormatDaySchedule(days[day]);
            if (hours.empty())
            {
                ++day;
                continue;
            }

            int endDay = day;
            while (endDay + 1 < 7 && SameSchedule(days[day], days[endDay + 1]))
                ++endDay;

            if (!result.empty())
                result += L';';
            AppendDayRange(result, day, endDay);
            result += L',';
            result += hours;
            day = endDay + 1;
        }
        return result;
    }
}

std::wstring FormatLogonHours(const LogonHours& hours)
{
    if (hours.All())
        return L"always allowed";

    WeekHours week{};
    hours.Get(week, false);
    const std::wstring schedule = FormatWeekSchedule(week);
    if (schedule.empty())
        return L"never allowed";
    return schedule;
}

std::wstring FormatAllUsersLogonHours(const std::map<std::wstring, LogonHours>& users)
{
    std::wstring result;
    for (const auto& it : users)
    {
        if (!result.empty())
            result += L"\r\n";
        result += it.first;
        result += L": ";
        result += FormatLogonHours(it.second);
    }
    if (result.empty())
        result = L"(no local user accounts)";
    return result;
}
