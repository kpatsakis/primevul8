libraw_processed_image_t *LibRaw::dcraw_make_mem_image(int *errcode)

{
  int width, height, colors, bps;
  get_mem_image_format(&width, &height, &colors, &bps);
  int stride = width * (bps / 8) * colors;
  unsigned ds = height * stride;
  libraw_processed_image_t *ret = (libraw_processed_image_t *)::malloc(
      sizeof(libraw_processed_image_t) + ds);
  if (!ret)
  {
    if (errcode)
      *errcode = ENOMEM;
    return NULL;
  }
  memset(ret, 0, sizeof(libraw_processed_image_t));

  // metadata init
  ret->type = LIBRAW_IMAGE_BITMAP;
  ret->height = height;
  ret->width = width;
  ret->colors = colors;
  ret->bits = bps;
  ret->data_size = ds;
  copy_mem_image(ret->data, stride, 0);

  return ret;
}