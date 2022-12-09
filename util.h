#ifndef __CCH_UTIL_H__
#define __CCH_UTIL_H__
#include<sys/types.h>
#include<pthread.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<stdint.h>

namespace cch {
	pid_t GetThreadId();
	uint32_t GetFiberId();
}

#endif // !__CCH_UTIL_H__


