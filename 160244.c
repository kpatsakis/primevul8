  DllDef int libraw_get_raw_width(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.raw_width;
  }