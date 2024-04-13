  libraw_memmgr(unsigned ee) : extra_bytes(ee)
  {
    size_t alloc_sz = LIBRAW_MSIZE * sizeof(void *);
    mems = (void **)::malloc(alloc_sz);
    memset(mems, 0, alloc_sz);
  }