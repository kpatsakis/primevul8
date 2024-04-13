  void free(void *ptr)
  {
    forget_ptr(ptr);
    ::free(ptr);
  }