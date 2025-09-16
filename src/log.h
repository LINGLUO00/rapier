# ifndef __RAPIER_LOG_H__
# define __RAPIER_LOG_H__

#include<cstdint>
#include<string>
#include<memory>
#include<list>
#include<sstream>//string stream
#include<fstream>
#include<vector>
#include<iostream>
#include<tuple>
# endif
namespace rapier {
class Logger;
//日志级别
class LogLevel {
public:
  enum Level{
    UNKNOW=0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
  };
  static const char* ToString(LogLevel::Level level);
};

//将日志封装为对象
class LogEvent{
public:
  typedef std::shared_ptr<LogEvent> ptr;
  LogEvent();
  const char* getFile() const{return _file;}
  int32_t getLine() const {return _line;}
  uint32_t getElapse() const{return _elapse;}
  uint32_t getThreadId() const {return _threadId;}
  uint32_t getFiberId() const {return _fiberId;}
  uint64_t getTime() const {return _time;}
  const std::string& getContent() const{return _content;}
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
  typedef std::shared_ptr<LogFormatter> ptr;
  LogFormatter(const std::string& pattern);
  std::string format (std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
  class FormatItem{
  public:
    typedef std::shared_ptr<FormatItem> ptr;
    virtual ~FormatItem()=default;
    virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
  };
  void init();
private:
  std::string _pattern;
  std::vector<FormatItem::ptr> _items;
};
//日志输出地
class LogAppender{
public:
  typedef std::shared_ptr<LogAppender> ptr;
  virtual ~LogAppender()=default;
  virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level,LogEvent::ptr) = 0;//纯虚函数，让子类必须实现这个方法。
  void setFormatter(LogFormatter::ptr val) {_formatter = val;}
  LogFormatter::ptr getFormatter() const {return _formatter;}
protected://需要给子类用到
  LogLevel::Level _level;
  LogFormatter::ptr _formatter;
};

//日志器
class Logger : public std::enable_shared_from_this<Logger> {//定义了这个，才能把自己作为智能指针传递给自己的成员函数
public:
  typedef std::shared_ptr<Logger> ptr;
  Logger(const std::string& name="root");
  void log(LogLevel::Level level, LogEvent::ptr event);
  void debug(LogEvent::ptr event);//这是语法糖，你可以用logger.debug(hahaha)来避免用log还要传level
  void info(LogEvent::ptr event);
  void warn(LogEvent::ptr event);
  void error(LogEvent::ptr event);
  void fatal(LogEvent::ptr event);
  const std::string& getName() const {return _name;}
  void addAppender(LogAppender::ptr appender);
  void delAppender(LogAppender::ptr appender);
  LogLevel::Level getLevel() const {return _level;}
  void setLevel(LogLevel::Level level) {_level = level;}

private:
  std::string _name;//日志名称
  LogLevel::Level _level;//日志级别
  std::list<LogAppender::ptr> _appenders;//Appender集合（输出到的目的地的集合）
  LogFormatter::ptr _formatter;                                               
};
//不同的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;//先把智能指针定义好，反正后面要直接用
    virtual void log(Logger::ptr logger, LogLevel::Level level,LogEvent::ptr event) override;
private:
};
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;//先把智能指针定义好，反正后面要直接用
    FileLogAppender(const std::string filename);
    virtual void log(Logger::ptr logger, LogLevel::Level level,LogEvent::ptr event) override;
    bool reopen();
private:
    std::string _filename;
    std::ofstream _filestream;
};
}
