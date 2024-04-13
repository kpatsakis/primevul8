  void set_exifparser_handler(exif_parser_callback cb, void *data)
  {
    callbacks.exifparser_data = data;
    callbacks.exif_cb = cb;
  }