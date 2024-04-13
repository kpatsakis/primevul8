  DllDef void libraw_set_fbdd_noiserd(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.fbdd_noiserd = value;
  }