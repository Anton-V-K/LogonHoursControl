#ifndef STDAFX_H_LogonHoursMonitorCP_
#define STDAFX_H_LogonHoursMonitorCP_

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <credentialprovider.h>
#include <objbase.h>
#include <LM.h>
#include <unknwn.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <atlbase.h>

// log4cpp must not be initialized inside LogonUI; stub logging for shared Common code.
struct CpLogNullStream
{
    template<typename T>
    CpLogNullStream& operator<<(const T&) { return *this; }
};
inline CpLogNullStream CpLogNull(const char*) { return {}; }

#define LOG_EMERG(x)   CpLogNull(x)
#define LOG_ALERT(x)   CpLogNull(x)
#define LOG_CRIT(x)    CpLogNull(x)
#define LOG_ERROR(x)   CpLogNull(x)
#define LOG_WARN(x)    CpLogNull(x)
#define LOG_NOTICE(x)  CpLogNull(x)
#define LOG_INFO(x)    CpLogNull(x)
#define LOG_DEBUG(x)   CpLogNull(x)

#endif
