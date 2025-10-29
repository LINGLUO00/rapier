#ifndef __RAPIER_UTIL_H__
#define __RAPIER_UTIL_H__

# include<sys/syscall.h>
#include<pthread.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdint.h>
namespace rapier{
  pid_t GetThreadId();
  uint32_t GetFiberId();
}

#endif
