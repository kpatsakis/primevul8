  void libraw_set_dataerror_handler(libraw_data_t *lr, data_callback func,
                                    void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_dataerror_handler(func, data);
  }