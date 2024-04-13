void LibRaw::leaf_hdr_load_raw()
{
  ushort *pixel = 0;
  unsigned tile = 0, r, c, row, col;

  if (!filters || !raw_image)
  {
    if (!image)
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    pixel = (ushort *)calloc(raw_width, sizeof *pixel);
    merror(pixel, "leaf_hdr_load_raw()");
  }
  try
  {
    FORC(tiff_samples)
    for (r = 0; r < raw_height; r++)
    {
      checkCancel();
      if (r % tile_length == 0)
      {
        fseek(ifp, data_offset + 4 * tile++, SEEK_SET);
        fseek(ifp, get4(), SEEK_SET);
      }
      if (filters && c != shot_select)
        continue;
      if (filters && raw_image)
        pixel = raw_image + r * raw_width;
      read_shorts(pixel, raw_width);
      if (!filters && image && (row = r - top_margin) < height)
        for (col = 0; col < width; col++)
          image[row * width + col][c] = pixel[col + left_margin];
    }
  }
  catch (...)
  {
    if (!filters)
      free(pixel);
    throw;
  }
  if (!filters)
  {
    maximum = 0xffff;
    raw_color = 1;
    free(pixel);
  }
}