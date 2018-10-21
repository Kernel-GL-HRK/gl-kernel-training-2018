#ifndef LESSON02_GIT_COMMON_H
#define LESSON02_GIT_COMMON_H

#define _DBG_PRINT_ 1

#ifdef _DBG_PRINT_
#include <cstdio> // getchar

#define DbgPrintI32(_x_) printf("DbgPrint: %d\n", (_x_));
#else
#define DbgPrintI32(_x_)
#endif

#endif
