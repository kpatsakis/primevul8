  ~libraw_memmgr()
  {
    cleanup();
    ::free(mems);
  }