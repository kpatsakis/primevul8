static void utf2char(utf16_t *str, char *buffer, unsigned bufsz)
{
  if (bufsz < 1)
    return;
  buffer[bufsz - 1] = 0;
  char *b = buffer;

  while (*str != 0x00 && --bufsz > 0)
  {
    char *chr = (char *)str;
    *b++ = *chr;
    str++;
  }
  *b = 0;
}