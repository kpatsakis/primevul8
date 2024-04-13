  DllDef void libraw_set_bright(libraw_data_t *lr, float value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.bright = value;
  }