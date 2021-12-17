#include <api/string.h>

void *memset(void *dest, int val, size_t length)
{
  unsigned char *ptr = dest;

  while (length-- > 0) {
    *ptr++ = val;
  }

  return dest;
}

size_t strlen(const char *str)
{
  size_t length = 0;
  while (str[length]) {
    length++;
  }

  return length;
}

