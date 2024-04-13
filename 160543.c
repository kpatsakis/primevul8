void LibRaw::free_image(void)
{
  if (imgdata.image)
  {
    free(imgdata.image);
    imgdata.image = 0;
    imgdata.progress_flags = LIBRAW_PROGRESS_START | LIBRAW_PROGRESS_OPEN |
                             LIBRAW_PROGRESS_IDENTIFY |
                             LIBRAW_PROGRESS_SIZE_ADJUST |
                             LIBRAW_PROGRESS_LOAD_RAW;
  }
}