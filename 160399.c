  int libraw_unpack_thumb(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->unpack_thumb();
  }