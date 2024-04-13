  void *malloc(size_t sz)
  {
#ifdef LIBRAW_USE_CALLOC_INSTEAD_OF_MALLOC
    void *ptr = ::calloc(sz + extra_bytes, 1);
#else
    void *ptr = ::malloc(sz + extra_bytes);
#endif
    mem_ptr(ptr);
    return ptr;
  }