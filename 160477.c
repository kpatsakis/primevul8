void LibRaw::kodak_thumb_load_raw()
{
  if (!image)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  int row, col;
  colors = thumb_misc >> 5;
  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
      read_shorts(image[row * width + col], colors);
  maximum = (1 << (thumb_misc & 31)) - 1;
}