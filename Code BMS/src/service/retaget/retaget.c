/*
 * retaget.c
 *
 *  Created on: Apr 2, 2023
 *      Author: MinhNhan
 */

#include "retaget.h"
#include "porting.h"

void retaget_init(void* arg){
    serial_init(arg);
}

#if defined(__GNUC__)
int _write(int fd, char * ptr, int len)
{
  (void)fd;
  (void)len;
  serial_puts(ptr);
  return len;
}
#elif defined (__ICCARM__)
#include "LowLevelIOInterface.h"
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  serial_puts(ptr);
  return size;
}
#elif defined (__CC_ARM)
int fputc(int ch, FILE *f)
{
    return ch;
}
#endif

// OR:

// Add syscalls.c with GCC

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  serial_putc((char)ch);
  return ch;
}
