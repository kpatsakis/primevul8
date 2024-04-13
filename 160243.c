void LibRaw::kodak_c330_load_raw()
{
  if (!image)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  uchar *pixel;
  int row, col, y, cb, cr, rgb[3], c;

  pixel = (uchar *)calloc(raw_width, 2 * sizeof *pixel);
  merror(pixel, "kodak_c330_load_raw()");
  try
  {
    for (row = 0; row < height; row++)
    {
      checkCancel();
      if (fread(pixel, raw_width, 2, ifp) < 2)
        derror();
      if (load_flags && (row & 31) == 31)
        fseek(ifp, raw_width * 32, SEEK_CUR);
      for (col = 0; col < width; col++)
      {
        y = pixel[col * 2];
        cb = pixel[(col * 2 & -4) | 1] - 128;
        cr = pixel[(col * 2 & -4) | 3] - 128;
        rgb[1] = y - ((cb + cr + 2) >> 2);
        rgb[2] = rgb[1] + cb;
        rgb[0] = rgb[1] + cr;
        FORC3 image[row * width + col][c] = curve[LIM(rgb[c], 0, 255)];
      }
    }
  }
  catch (...)
  {
    free(pixel);
    throw;
  }
  free(pixel);
  maximum = curve[0xff];
}