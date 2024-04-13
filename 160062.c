  int libraw_open_file(libraw_data_t *lr, const char *file)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file);
  }