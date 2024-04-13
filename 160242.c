void LibRaw::eight_bit_load_raw()
{
  uchar *pixel;
  unsigned row, col;

  pixel = (uchar *)calloc(raw_width, sizeof *pixel);
  merror(pixel, "eight_bit_load_raw()");
  try
  {
    for (row = 0; row < raw_height; row++)
    {
      checkCancel();
      if (fread(pixel, 1, raw_width, ifp) < raw_width)
        derror();
      for (col = 0; col < raw_width; col++)
        RAW(row, col) = curve[pixel[col]];
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