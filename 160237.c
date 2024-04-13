  int libraw_dcraw_thumb_writer(libraw_data_t *lr, const char *fname)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_thumb_writer(fname);
  }