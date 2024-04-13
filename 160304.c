void write_ppm(libraw_processed_image_t *img, const char *basename)
{
  if (!img)
    return;
  // type SHOULD be LIBRAW_IMAGE_BITMAP, but we'll check
  if (img->type != LIBRAW_IMAGE_BITMAP)
    return;
  // only 3-color images supported...
  if (img->colors != 3)
    return;

  char fn[1024];
  snprintf(fn, 1024, "%s.ppm", basename);
  FILE *f = fopen(fn, "wb");
  if (!f)
    return;
  fprintf(f, "P6\n%d %d\n%d\n", img->width, img->height, (1 << img->bits) - 1);
/*
  NOTE:
  data in img->data is not converted to network byte order.
  So, we should swap values on some architectures for dcraw compatibility
  (unfortunately, xv cannot display 16-bit PPMs with network byte order data
*/
#define SWAP(a, b)                                                             \
  {                                                                            \
    a ^= b;                                                                    \
    a ^= (b ^= a);                                                             \
  }
  if (img->bits == 16 && htons(0x55aa) != 0x55aa)
    for (unsigned i = 0; i < img->data_size; i += 2)
      SWAP(img->data[i], img->data[i + 1]);
#undef SWAP

  fwrite(img->data, img->data_size, 1, f);
  fclose(f);
}