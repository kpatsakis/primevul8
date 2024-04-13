  int libraw_open_buffer(libraw_data_t *lr, void *buffer, size_t size)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_buffer(buffer, size);
  }