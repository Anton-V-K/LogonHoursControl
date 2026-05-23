#ifndef STDAFX_H_LogonHoursMonitor_
#define STDAFX_H_LogonHoursMonitor_

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shellapi.h>

#include <LM.h>
#include <tchar.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlcrack.h>
#include <atlwin.h>

#include <log4cpp/DailyRollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Win32DebugAppender.hh>

#include <Common/Logger.h>

#endif
