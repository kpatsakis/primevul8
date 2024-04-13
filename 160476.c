void LibRaw::packed_dng_load_raw()
{
  ushort *pixel, *rp;
  int row, col;

  int ss = shot_select;
  shot_select = libraw_internal_data.unpacker_data.dng_frames[LIM(ss,0,(LIBRAW_IFD_MAXCOUNT*2-1))] & 0xff;

  pixel = (ushort *)calloc(raw_width, tiff_samples * sizeof *pixel);
  merror(pixel, "packed_dng_load_raw()");
  try
  {
    for (row = 0; row < raw_height; row++)
    {
      checkCancel();
      if (tiff_bps == 16)
        read_shorts(pixel, raw_width * tiff_samples);
      else
      {
        getbits(-1);
        for (col = 0; col < raw_width * tiff_samples; col++)
          pixel[col] = getbits(tiff_bps);
      }
      for (rp = pixel, col = 0; col < raw_width; col++)
        adobe_copy_pixel(row, col, &rp);
    }
  }
  catch (...)
  {
    free(pixel);
    shot_select = ss;
    throw;
  }
  free(pixel);
  shot_select = ss;
}