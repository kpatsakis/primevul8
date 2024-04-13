  void default_memory_callback(void *, const char *file, const char *where)
  {
    fprintf(stderr, "%s: Out of memory in %s\n", file ? file : "unknown file",
            where);
  }