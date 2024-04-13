  void libraw_set_progress_handler(libraw_data_t *lr, progress_callback cb,
                                   void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_progress_handler(cb, data);
  }