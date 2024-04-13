  void libraw_subtract_black(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->subtract_black();
  }