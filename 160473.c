  void libraw_recycle_datastream(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->recycle_datastream();
  }