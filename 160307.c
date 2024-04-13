void LibRaw::unpacked_load_raw_reversed()
{
  int row, col, bits = 0;
  while (1 << ++bits < maximum)
    ;
  for (row = raw_height - 1; row >= 0; row--)
  {
    checkCancel();
    read_shorts(&raw_image[row * raw_width], raw_width);
    for (col = 0; col < raw_width; col++)
      if ((RAW(row, col) >>= load_flags) >> bits &&
          (unsigned)(row - top_margin) < height &&
          (unsigned)(col - left_margin) < width)
        derror();
  }
}