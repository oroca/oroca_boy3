/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>
#include "uart.h"


#define FreeRTOS
#define MAX_STACK_SIZE 0x2000

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

#ifndef FreeRTOS
  register char * stack_ptr asm("sp");
#endif


static char *heap_end = 0;


void _printHeapInfo(void)
{
  extern char end asm("end");
  char *min_stack_ptr;
  char *heap_end_in;


  /* Use the NVIC offset register to locate the main stack pointer. */
  min_stack_ptr = (char*)(*(unsigned int *)*(unsigned int *)0xE000ED08);

  if (heap_end == 0)
    heap_end_in = &end;


  printf("heap %X, %X, %dK\n",
         (int)heap_end_in,
         (int)min_stack_ptr,
         ((int)min_stack_ptr - (int)heap_end_in)/1024);
}

uint32_t _getHeapFree(void)
{
  extern char end asm("end");
  char *min_stack_ptr;
  char *heap_end_in;

  /* Use the NVIC offset register to locate the main stack pointer. */
  min_stack_ptr = (char*)(*(unsigned int *)*(unsigned int *)0xE000ED08);

  if (heap_end == 0)
    heap_end_in = &end;

  if (min_stack_ptr > heap_end_in)
  {
    return (min_stack_ptr - heap_end_in);
  }
  else
  {
    return 0;
  }
}

caddr_t _sbrk(int incr)
{
  extern char end asm("end");
  char *prev_heap_end,*min_stack_ptr;

  if (heap_end == 0)
    heap_end = &end;

  prev_heap_end = heap_end;

#ifdef FreeRTOS
  /* Use the NVIC offset register to locate the main stack pointer. */
  min_stack_ptr = (char*)(*(unsigned int *)*(unsigned int *)0xE000ED08);
  /* Locate the STACK bottom address */
  min_stack_ptr -= MAX_STACK_SIZE;

  //uartPrintf(_DEF_UART1, "heap free %X %X %X, %dK, %dK\n", heap_end, incr, min_stack_ptr, incr/1024,(int)(min_stack_ptr - heap_end)/1024);
  if (heap_end + incr > min_stack_ptr)
#else
  if (heap_end + incr > stack_ptr)
#endif
  {
//    write(1, "Heap and stack collision\n", 25);
//    abort();
    printf("malloc fail %X %X %X\n", (int)heap_end, (int)incr, (int)min_stack_ptr);
    errno = ENOMEM;
    return (caddr_t) -1;
  }

  heap_end += incr;

  return (caddr_t) prev_heap_end;
}

/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday (struct timeval * tp, struct timezone * tzp)
{
  /* Return fixed data for the timezone.  */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}
void initialise_monitor_handles()
{
}

int _getpid(void)
{
  return 1;
}

int _kill(int pid, int sig)
{
  errno = EINVAL;
  return -1;
}

void _exit (int status)
{
  _kill(status, -1);
  while (1) {}
}

int _write(int file, char *ptr, int len)
{
  int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
       __io_putchar( *ptr++ );
    }
  return len;
}

int _close(int file)
{
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _read(int file, char *ptr, int len)
{
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    *ptr++ = __io_getchar();
  }

   return len;
}

int _open(char *path, int flags, ...)
{
  /* Pretend like we always fail */
  return -1;
}

int _wait(int *status)
{
  errno = ECHILD;
  return -1;
}

int _unlink(char *name)
{
  errno = ENOENT;
  return -1;
}

int _times(struct tms *buf)
{
  return -1;
}

int _stat(char *file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _link(char *old, char *new)
{
  errno = EMLINK;
  return -1;
}

int _fork(void)
{
  errno = EAGAIN;
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}
