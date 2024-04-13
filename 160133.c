  DllDef void libraw_set_demosaic(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.user_qual = value;
  }