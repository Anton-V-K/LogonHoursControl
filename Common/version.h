#ifndef CPC_version_h_
#define CPC_version_h_

#ifdef _DEBUG
#  define VERSION_CONFIG " Debug"
#else
#  define VERSION_CONFIG
#endif

// NOTE: these numbers are parsed in _Release.cmd,
// so do not change the format of these lines
#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_REV   0

#define VERSION_STRING "1.2.0 Alpha" VERSION_CONFIG

#endif
