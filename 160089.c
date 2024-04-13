  void libraw_recycle(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->recycle();
  }