  DllDef int libraw_get_raw_height(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.raw_height;
  }