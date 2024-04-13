  virtual int get_char()
  {
    if (streampos >= streamsize)   return -1;
    return buf[streampos++];
  }