  libraw_processed_image_t *libraw_dcraw_make_mem_image(libraw_data_t *lr,
                                                        int *errc)
  {
    if (!lr)
    {
      if (errc)
        *errc = EINVAL;
      return NULL;
    }
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_make_mem_image(errc);
  }