#include "../src/log.h"
#include "../src/util.h"
#include <cassert>
#include <cstring>
#include <iostream>

using namespace rapier;

int main(int argc, char** argv) {
  rapier::Logger::ptr logger(new rapier::Logger);
  logger->addAppender(rapier::LogAppender::ptr(new rapier::StdoutLogAppender));
  //rapier::LogEvent::ptr event(new rapier::LogEvent(__FILE__, __LINE__, 0,rapier::GetThreadId() ,rapier::GetFiberId(),time(0)));
  //logger->log(rapier::LogLevel::DEBUG, event);
  rapier::FileLogAppender::ptr file_appender(new rapier::FileLogAppender("./log.txt"));
  rapier::LogFormatter::ptr fmt(new rapier::LogFormatter("%d%T%p%T%m%n"));
  file_appender->setFormatter(fmt);
  file_appender->setLevel(rapier::LogLevel::ERROR);
  logger->addAppender(file_appender);
  RAPIER_LOG_INFO(logger)<<"test macro";
  RAPIER_LOG_FMT_ERROR(logger,"test macro fmt error %s","hahaha");
  auto i = rapier::LoggerMgr::GetInstance()->getLogger("xx");
  RAPIER_LOG_INFO(i)<<"xxx";
  return 0;
}
