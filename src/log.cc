#include "log.h"
#include <functional>
#include<map>
#include<string>
#include<iostream>
#include<time.h>
namespace rapier{

  const char* LogLevel::ToString(LogLevel::Level level){
   switch(level){
  # define XX(name) \
     case LogLevel::name: \
      return #name; \
      break;
     XX(DEBUG);
     XX(INFO);
     XX(WARN);
     XX(ERROR);
     XX(FATAL);
  # undef XX
     default:
      return "UNKNOW";
   }
  }


  class MessageFormatItem:public LogFormatter::FormatItem{
  public:
    MessageFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{ 
      os<<event->getContent();
    }

  };
  class LevelFormatItem:public LogFormatter::FormatItem{
  public:
    LevelFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<LogLevel::ToString(level);
    }

  };
  class NameFormatItem:public LogFormatter::FormatItem{
  public:
    NameFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<logger->getName();
    }
  };
  class ElapseFormatItem : public LogFormatter::FormatItem{
  public:
    ElapseFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<event->getElapse();
    } 
  };
  class ThreadIdFormatItem : public LogFormatter::FormatItem{
  public:
    ThreadIdFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<event->getThreadId();
    } 
  }; 
  class FiberIdFormatItem : public LogFormatter::FormatItem{
  public:
    FiberIdFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<event->getFiberId();
    } 
  };
  class DateTimeFormatItem : public LogFormatter::FormatItem{
  public:
    DateTimeFormatItem(const std::string& format="%Y-%m-%d %H:%M:&S")
      :_format(format){
        if(_format.empty()) _format="%Y-%m-%d %H:%M:%S";
      }
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      struct tm tm;
      time_t time = event->getTime();
      localtime_r(&time, &tm);
      char buf[64];
      strftime(buf, sizeof(buf), _format.c_str(), &tm);
      os<<buf;
    }
  private:
    std::string _format;
  };
  class FilenameFormatItem : public LogFormatter::FormatItem{
  public:
    FilenameFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<event->getFile();
    } 
  };
  class LineFormatItem : public LogFormatter::FormatItem{
  public:
    LineFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<event->getLine();
    } 
  };
  class StringFormatItem : public LogFormatter::FormatItem{
  public:
    StringFormatItem(const std::string& str)
      :_string(str){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<_string;
    } 
  private:
    std::string _string;
  };

  class TabFormatItem : public LogFormatter::FormatItem{
  public:
    TabFormatItem(const std::string& str = ""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<"\t";
    } 
  private:
    std::string _string;
  };
  
  LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse
      , uint32_t thread_id, uint32_t fiber_id, uint64_t time)
  :_line(line)
  ,_file(file)
  ,_time(time)
  ,_elapse(elapse)
  ,_threadId(thread_id)
  ,_fiberId(fiber_id)
  ,_logger(logger)
  ,_level(level){
  }
  
  LogEvent::~LogEvent(){};

  void LogEvent::format(const char* fmt, ...){
    va_list ap;
    va_start(ap,fmt);
    format(fmt,ap);
    va_end(ap);
  }

  void LogEvent::format(const char* fmt, va_list ap){
    char* strp=nullptr;
    int len = vasprintf(&strp,fmt,ap);
    if(len!=-1){
      _ss<<std::string(strp,len);
      free(strp);
    }
  }

  LogEventWrap::LogEventWrap(LogEvent::ptr event)
  :_event(event){
  }
  LogEventWrap::~LogEventWrap(){
    _event->getLogger()->log(_event->getLevel(),_event);
  }
  std::stringstream& LogEventWrap::getSS(){
    return _event->getSS();
  }

  class NewLineFormatItem : public LogFormatter::FormatItem{
  public:
    NewLineFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<std::endl;
    } 
  };


  Logger::Logger(const std::string& name)
    : _name(name)
    ,_level(LogLevel::DEBUG) {
      _formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

  }
  void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= _level){
      auto self = shared_from_this();
      for(auto& i : _appenders){
        i->log(self, level,event);
      }
    }
  }
  void Logger::debug(LogEvent::ptr event){
    Logger::log(LogLevel::DEBUG,event);
  }
  void Logger::info(LogEvent::ptr event){
    Logger::log(LogLevel::INFO,event);
  }
  void Logger::warn(LogEvent::ptr event){
    Logger::log(LogLevel::WARN,event);
  }
  void Logger::error(LogEvent::ptr event){
    Logger::log(LogLevel::ERROR,event);
  }
  void Logger::fatal(LogEvent::ptr event){
    Logger::log(LogLevel::FATAL,event);
  }

  void Logger::addAppender(LogAppender::ptr appender){
    if(!appender->getFormatter()){
      appender->setFormatter(_formatter);
    }
    _appenders.push_back(appender);
  }
  void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = _appenders.begin();it != _appenders.end();it++){
      if(*it == appender){
        _appenders.erase(it);
        break;
      }
    }
  }

  FileLogAppender::FileLogAppender(const std::string filename)
    :_filename(filename){
    reopen();
  }
  void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= _level){
      _filestream<<_formatter->format(logger, level, event);
    }
  }

  bool FileLogAppender::reopen(){
    if(_filestream){//值为真说明有效
      _filestream.close();
    }
    _filestream.open(_filename);
    return !!_filestream;//cpp中!!是把所有可以条件化的东西转化为标准的bool值;

  }
  void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= _level){
      std::cout<< _formatter->format(logger, level, event); 
    }
  }
  LogFormatter::LogFormatter(const std::string& pattern)
    :_pattern(pattern){
    init();
  }
  std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto& i : _items){
      i->format(ss,logger,level,event);
    }
    return ss.str();
  }
  void LogFormatter::init(){
    std::vector<std::tuple<std::string,std::string,int>> vec;//string, format, type
    std::string str;//normal string
    //解析的格式为%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    for(size_t i=0; i<_pattern.size();i++){
      if(_pattern[i]!='%'){
        str.append(1,_pattern[i]);
        continue;
      }
      if((i+1)<_pattern.size() && _pattern[i+1]=='%'){  
        str.append(1,'%');
        continue;
      }
      std::string placeHolder;
      std::string fmt;
      size_t n =i+1;//n从‘%’后的第一个字符开始，比如d
      int fmt_status=0;
      size_t fmt_begin=0;
      while(n<_pattern.size()){
        //在花括号‘{'之前，遇到非字符，非左右花括号就退出
        if(fmt_status==0 && !isalpha(_pattern[n]) && _pattern[n] != '{' && _pattern[n] != '}'){//isalpha是判断是否为字母
          break;//从这里就出去的话，n指向的是‘%’后的第一个非字符，非左右花括号，比如空格，‘['等等，而i是d/m/p等等
        }
        if(fmt_status==0 && _pattern[n]=='{'){
            placeHolder = _pattern.substr(i+1,n-i-1);//这里的i+1是‘%'的位置，n在’{'的位置上，中间刚好的d/m等等
            fmt_status=1;
            fmt_begin=n;
            n++;
            continue;
          }
        if(fmt_status ==1 && _pattern[n]=='}'){
          fmt = _pattern.substr(fmt_begin+1,n-fmt_begin-1);
          fmt_status=2;
          break;//退出的时候，n为‘}’，i为‘%’前一位
        }
       ++n;//第一次会把n加到’{'的位置上
      }
      if(fmt_status==0){//这个部分是处理“循环在遇到 左花括号之前 就已经 break 出来了”，所以现在的n指向的是‘%’后的第一个字符，比如空格，‘['等等
        if(!str.empty()){
          vec.push_back(std::make_tuple(str,std::string(),0));
          str.clear();
        }
        placeHolder=_pattern.substr(i+1,n-i-1);//这里得到的就是d/m等等
        vec.push_back(std::make_tuple(placeHolder,fmt,1));
        i=n-1;
      }else if(fmt_status==1){//说明没找到右花括号
        std::cout<<"pattern parse error: "<<_pattern<<" - "<<_pattern.substr(i)<<std::endl;
        vec.push_back(std::make_tuple("<<pattern_error>>",fmt,0));
      }else if(fmt_status==2){//说明成功完成了匹配
        if(!str.empty()){
          vec.push_back(std::make_tuple(str,std::string(),0));
          str.clear();
        }
        vec.push_back(std::make_tuple(placeHolder,fmt,1));//这里的str是d/m等等，fmt是格式，正常情况都是只走这个分支
        i=n;//把i移动到‘}’，因为下一轮for循环会++i，所以i会指向‘}’后一个位置
      }
    }
    if(!str.empty()){//如果pattern以普通字符结尾，那么nstr的内容会在这里被push,因为for循环没有下一轮了
      vec.push_back(std::make_tuple(str,"",0));
    }
    //%m--消息体
    //%p--level
    //%r--启动后的时间
    //%c--日志名称
    //%t--线程id
    //%n--回车换行
    //%d--时间
    //%f--file name
    //%l--line
  static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items{
#define XX(str,C) \
    {str,[](const std::string& fmt) {return FormatItem::ptr(new C(fmt));}}
    XX("m",MessageFormatItem),
    XX("p",LevelFormatItem),
    XX("r",ElapseFormatItem),
    XX("c",NameFormatItem),
    XX("n",NewLineFormatItem),
    XX("d",DateTimeFormatItem),
    XX("f",FilenameFormatItem),
    XX("l",LineFormatItem),
    XX("T",TabFormatItem),
    XX("F",FiberIdFormatItem),
    XX("t",ThreadIdFormatItem),
#undef XX
  };

  for(auto& i : vec){
    if(std::get<2>(i)==0){
      _items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
    }else{
      auto it = s_format_items.find(std::get<0>(i));
      if(it == s_format_items.end()){
        _items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
      }else{
        _items.push_back(it->second(std::get<1>(i)));//在这里将每一个item要格式化的格式作为参数传进去
      }
    }
    //std::cout<<"("<<std::get<0>(i)<<")"<<" - "<<"("<<std::get<1>(i)<<")"<<" - "<<std::get<2>(i)<<std::endl;
  }
}

LoggerManager::LoggerManager(){
  _root.reset(new Logger);
  _root->addAppender(LogAppender::ptr(new StdoutLogAppender));
}
Logger::ptr LoggerManager::getLogger(const std::string& name){
  auto it = _loggers.find(name);
  return it==_loggers.end() ? _root : it->second;
}
void init();
}
                                        
