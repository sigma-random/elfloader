#ifndef __DEBUG__
#define __DEBUG__
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#define __DEBUG_INFO__
#ifdef  __DEBUG_INFO__
    #define dbg_printf  printf
#else
    #define dbg_printf ;//
#endif

static int logger(FILE *fd, const char *format, ...)  {
    va_list argptr;
    int cnt = 0;

    va_start(argptr, format);
    if( stderr == fd ) { 
        fprintf(fd, "[Err]  ");
        cnt = vfprintf(fd, format, argptr);  
    }else {
    #ifdef __LOG_INFO__
        fprintf(fd, "[Info] ");
        cnt = vfprintf(fd, format, argptr);  
    #endif
    }   
    va_end(argptr);
    return cnt;
}


#endif
