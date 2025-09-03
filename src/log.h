# ifndef __RAPIER_LOG_H__
# define __RAPIER_LOG_H__

#include<cstdint>
#include<string>
#include<memory>
# endif
namespace RapierLog {
//日志级别
class LogLevel {
public:
  enum Level{
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    FATAL = 4,
  };
};

//将日志封装为对象
class LogEvent{
public:
  typedef std::shared_ptr<LogEvent> eventPtr;
  LogEvent();
private:
  int32_t _line = 0; //行号
  const char* _file = nullptr; //文件名
  uint64_t _time = 0; //时间戳
  std::string _content; //文本内容
  uint32_t _elapse = 0;  //程序启动到现在的毫秒数
  uint32_t _threadId = 0; //线程id
  uint32_t _fiberId = 0;                          //协程id
};

//日志格式器
class LogFormatter{
public:
  typedef std::shared_ptr<LogFormatter> formatterPtr;
  std::string formate (LogEvent::eventPtr event);
};
//日志输出地
class LogAppender{
public:
  typedef std::shared_ptr<LogAppender> appenderPtr;
  virtual ~LogAppender();
  void log(LogLevel::Level,LogEvent::eventPtr);
private:
  LogLevel::Level _level;
};

//日志器
class Logger{
public:
  typedef std::shared_ptr<Logger> loggerPtr;
  Logger(const std::string& name="root");
  void log(LogLevel::Level level, LogEvent::eventPtr);
private:
  std::string _name;
  LogLevel::Level _level;
  LogAppender::appenderPtr _appenderPtr;
};
//不同的Appender
class StdoutLogAppender : public LogAppender{
};
class FileLogAppender : public LogAppender{
};
}
