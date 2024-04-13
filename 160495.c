  DllDef int libraw_get_iheight(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.iheight;
  }