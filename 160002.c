  DllDef float libraw_get_cam_mul(libraw_data_t *lr, int index)
  {
    if (!lr)
      return EINVAL;
    return lr->color.cam_mul[LIM(index, 0, 3)];
  }