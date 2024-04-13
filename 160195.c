  DllDef int libraw_get_color_maximum(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->color.maximum;
  }