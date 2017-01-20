#ifndef LOG__H
#define LOG__H
#include <iostream>

enum LogLevel { trace, debug, info, warning, error, fatal };

////////////// LOG LEVEL SETTING /////////////////////////
#define LOG_LEVEL debug
//////////////////////////////////////////////////////////

static std::ostream nullout(nullptr);
#define LOG(level) (level >= LOG_LEVEL ? cerr : nullout)

#endif
