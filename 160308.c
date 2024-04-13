  DllDef void libraw_set_gamma(libraw_data_t *lr, int index, float value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.gamm[LIM(index, 0, 5)] = value;
  }