  int libraw_adjust_sizes_info_only(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->adjust_sizes_info_only();
  }