#ifndef LOG__H
#define LOG__H
#include <iostream>

enum LogLevel { trace, debug, info, warning, error, fatal };

////////////// LOG LEVEL SETTING /////////////////////////
#define LOG_LEVEL info
//////////////////////////////////////////////////////////

static std::ostream nullout(nullptr);
#define LOG(level) (level >= LOG_LEVEL ? std::cerr : nullout)

#endif
