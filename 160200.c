  void libraw_free_image(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->free_image();
  }