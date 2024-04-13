  DllDef void libraw_set_no_auto_bright(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.no_auto_bright = value;
  }