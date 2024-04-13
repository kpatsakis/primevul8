  void mem_ptr(void *ptr)
  {
    if (ptr)
    {
      for (int i = 0; i < LIBRAW_MSIZE - 1; i++)
        if (!mems[i])
        {
          mems[i] = ptr;
          return;
        }
#ifdef LIBRAW_MEMPOOL_CHECK
      /* remember ptr in last mems item to be free'ed at cleanup */
      if (!mems[LIBRAW_MSIZE - 1])
        mems[LIBRAW_MSIZE - 1] = ptr;
      throw LIBRAW_EXCEPTION_MEMPOOL;
#endif
    }
  }