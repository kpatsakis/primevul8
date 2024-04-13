  DllDef int libraw_get_iwidth(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.iwidth;
  }