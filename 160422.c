  void cleanup(void)
  {
    for (int i = 0; i < LIBRAW_MSIZE; i++)
      if (mems[i])
      {
        ::free(mems[i]);
        mems[i] = NULL;
      }
  }