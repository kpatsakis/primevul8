  void *realloc(void *ptr, size_t newsz)
  {
    void *ret = ::realloc(ptr, newsz + extra_bytes);
    forget_ptr(ptr);
    mem_ptr(ret);
    return ret;
  }