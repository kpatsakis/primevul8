  DllDef float libraw_get_rgb_cam(libraw_data_t *lr, int index1, int index2)
  {
    if (!lr)
      return EINVAL;
    return lr->color.rgb_cam[LIM(index1, 0, 2)][LIM(index2, 0, 3)];
  }