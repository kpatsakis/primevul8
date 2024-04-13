  int libraw_get_decoder_info(libraw_data_t *lr, libraw_decoder_info_t *d)
  {
    if (!lr || !d)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->get_decoder_info(d);
  }