#include <api/string.h>

void *memset(void *dest, int val, size_t length)
{
  unsigned char *ptr = dest;

  while (length-- > 0) {
    *ptr++ = val;
  }

  return dest;
}
