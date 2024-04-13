array_copy(mrb_value *dst, const mrb_value *src, mrb_int size)
{
  mrb_int i;

  for (i = 0; i < size; i++) {
    dst[i] = src[i];
  }
}