#ifndef LOGSTREAMHEADER
#define LOGSTREAMHEADER
#include <assert.h>
#include <string.h> // memcpy

namespace muduo
{
class SourceFile
{

};
enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

class Logger
{

public:
    static LogLevel getLogLevel();
    static void setLogLevel(LogLevel level);

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

};





}



#endif