  libraw_lensinfo_t *libraw_get_lensinfo(libraw_data_t *lr)
  {
    if (!lr)
      return NULL;
    return &(lr->lens);
  }