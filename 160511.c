void LibRaw::imacon_full_load_raw()
{
  if (!image)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  int row, col;

  unsigned short *buf =
      (unsigned short *)malloc(width * 3 * sizeof(unsigned short));
  merror(buf, "imacon_full_load_raw");

  for (row = 0; row < height; row++)
  {
    checkCancel();
    read_shorts(buf, width * 3);
    unsigned short(*rowp)[4] = &image[row * width];
    for (col = 0; col < width; col++)
    {
      rowp[col][0] = buf[col * 3];
      rowp[col][1] = buf[col * 3 + 1];
      rowp[col][2] = buf[col * 3 + 2];
      rowp[col][3] = 0;
    }
  }
  free(buf);
}