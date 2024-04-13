  static unsigned sgetn(int n, uchar *s)
  {
    unsigned result = 0;
    while (n-- > 0)
      result = (result << 8) | (*s++);
    return result;
  }