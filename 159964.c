  int libraw_open_wfile(libraw_data_t *lr, const wchar_t *file)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file);
  }