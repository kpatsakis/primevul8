  libraw_imgother_t *libraw_get_imgother(libraw_data_t *lr)
  {
    if (!lr)
      return NULL;
    return &(lr->other);
  }