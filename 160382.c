int LibRaw::adjust_maximum()
{
  ushort real_max;
  float auto_threshold;

  if (O.adjust_maximum_thr < 0.00001)
    return LIBRAW_SUCCESS;
  else if (O.adjust_maximum_thr > 0.99999)
    auto_threshold = LIBRAW_DEFAULT_ADJUST_MAXIMUM_THRESHOLD;
  else
    auto_threshold = O.adjust_maximum_thr;

  real_max = C.data_maximum;
  if (real_max > 0 && real_max < C.maximum &&
      real_max > C.maximum * auto_threshold)
  {
    C.maximum = real_max;
  }
  return LIBRAW_SUCCESS;
}