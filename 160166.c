  const char *libraw_unpack_function_name(libraw_data_t *lr)
  {
    if (!lr)
      return "NULL parameter passed";
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->unpack_function_name();
  }