  int libraw_dcraw_process(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_process();
  }