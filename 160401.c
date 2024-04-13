  int libraw_raw2image(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->raw2image();
  }