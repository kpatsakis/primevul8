  libraw_iparams_t *libraw_get_iparams(libraw_data_t *lr)
  {
    if (!lr)
      return NULL;
    return &(lr->idata);
  }