  void forget_ptr(void *ptr)
  {
    if (ptr)
      for (int i = 0; i < LIBRAW_MSIZE; i++)
        if (mems[i] == ptr)
        {
          mems[i] = NULL;
          break;
        }
  }