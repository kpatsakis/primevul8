  int libraw_open_wfile_ex(libraw_data_t *lr, const wchar_t *file, INT64 sz)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file, sz);
  }