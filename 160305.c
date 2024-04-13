  void *calloc(size_t n, size_t sz)
  {
    void *ptr = ::calloc(n + (extra_bytes + sz - 1) / (sz ? sz : 1), sz);
    mem_ptr(ptr);
    return ptr;
  }