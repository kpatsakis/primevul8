  void libraw_set_exifparser_handler(libraw_data_t *lr, exif_parser_callback cb,
                                     void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_exifparser_handler(cb, data);
  }