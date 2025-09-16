#include "log.h"
#include <functional>
#include<map>
#include<string>
#include<iostream>
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
    DateTimeFormatItem(const std::string& format="%Y:%m:%d %H:%M:&S")
      :_format(format){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<event->getTime();
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
  class NewLineFormatItem : public LogFormatter::FormatItem{
  public:
    NewLineFormatItem(const std::string& str=""){}
    void format(std::ostream& os, Logger::ptr logger,LogLevel::Level level, LogEvent::ptr event) override{
      os<<std::endl;
    } 
  };


  Logger::Logger(const std::string& name)
    : _name(name){
      _formatter.reset(new LogFormatter("%d [%p] %f %l %m %n"));

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
    std::string nstr;
    for(size_t i=0; i<_pattern.size();i++){
      if(_pattern[i]!='%'){
        nstr.append(1,_pattern[i]);
        continue;
      }
      if((i+1)<_pattern.size()){
        if(_pattern[i+1]=='%'){
          nstr.append(1,'%');
          continue;
        }
      }
      std::string str;
      std::string fmt;
      size_t n =i+1;
      int fmt_status=0;
      size_t fmt_begin=0;
      while(n<_pattern.size()){
        if(isspace(_pattern[n])){
          break;
        }
        if(fmt_status==0 && _pattern[n]=='{'){
            str = _pattern.substr(i+1,n-i-1);
            fmt_status=1;
            fmt_begin=n;
            n++;
            continue;
          }
        if(fmt_status ==1 && _pattern[n]=='}'){
          fmt = _pattern.substr(fmt_begin+1,n-fmt_begin-1);
          fmt_status=2;
          break;
        }
      }
      if(fmt_status==0){
        if(!nstr.empty()){
          vec.push_back(std::make_tuple(nstr,std::string(),0));
        }
        str=_pattern.substr(i+1,n-i-1);
        vec.push_back(std::make_tuple(str,fmt,1));
        i=n;
      }else if(fmt_status==1){
        std::cout<<"pattern parse error: "<<_pattern<<" - "<<_pattern.substr(i)<<std::endl;
        vec.push_back(std::make_tuple("<<pattern_error>>",fmt,0));
      }else if(fmt_status==2){
        if(!nstr.empty()){
          vec.push_back(std::make_tuple(nstr,std::string(),0));
        }
        vec.push_back(std::make_tuple(str,fmt,1));
        i=n;
      }
    }
    if(!nstr.empty()){
      vec.push_back(std::make_tuple(nstr,"",0));
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
        _items.push_back(it->second(std::get<1>(i)));
      }
    }
    std::cout<<std::get<0>(i)<<" - "<<std::get<1>(i)<<" - "<<std::get<2>(i)<<std::endl;
  }
}
}
                                        
