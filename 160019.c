  DllDef void libraw_set_user_mul(libraw_data_t *lr, int index, float val)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.user_mul[LIM(index, 0, 3)] = val;
  }