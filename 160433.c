  virtual int get_char()
  {
#ifndef LIBRAW_WIN32_CALLS
    return getc_unlocked(f);
#else
    return fgetc(f);
#endif
  }