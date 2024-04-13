  DllDef float libraw_get_pre_mul(libraw_data_t *lr, int index)
  {
    if (!lr)
      return EINVAL;
    return lr->color.pre_mul[LIM(index, 0, 3)];
  }