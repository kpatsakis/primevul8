void write_thumb(libraw_processed_image_t *img, const char *basename)
{
  if (!img)
    return;

  if (img->type == LIBRAW_IMAGE_BITMAP)
  {
    char fnt[1024];
    snprintf(fnt, 1024, "%s.thumb", basename);
    write_ppm(img, fnt);
  }
  else if (img->type == LIBRAW_IMAGE_JPEG)
  {
    char fn[1024];
    snprintf(fn, 1024, "%s.thumb.jpg", basename);
    FILE *f = fopen(fn, "wb");
    if (!f)
      return;
    fwrite(img->data, img->data_size, 1, f);
    fclose(f);
  }
}