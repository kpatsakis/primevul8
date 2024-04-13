  int libraw_COLOR(libraw_data_t *lr, int row, int col)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->COLOR(row, col);
  }