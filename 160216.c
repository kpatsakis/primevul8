  void libraw_set_memerror_handler(libraw_data_t *lr, memory_callback cb,
                                   void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_memerror_handler(cb, data);
  }