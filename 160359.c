  int libraw_open_file_ex(libraw_data_t *lr, const char *file, INT64 sz)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file, sz);
  }