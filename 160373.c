void LibRaw::fill_holes(int holes)
{
  int row, col, val[4];

  for (row = 2; row < height - 2; row++)
  {
    if (!HOLE(row))
      continue;
    for (col = 1; col < width - 1; col += 4)
    {
      val[0] = RAW(row - 1, col - 1);
      val[1] = RAW(row - 1, col + 1);
      val[2] = RAW(row + 1, col - 1);
      val[3] = RAW(row + 1, col + 1);
      RAW(row, col) = median4(val);
    }
    for (col = 2; col < width - 2; col += 4)
      if (HOLE(row - 2) || HOLE(row + 2))
        RAW(row, col) = (RAW(row, col - 2) + RAW(row, col + 2)) >> 1;
      else
      {
        val[0] = RAW(row, col - 2);
        val[1] = RAW(row, col + 2);
        val[2] = RAW(row - 2, col);
        val[3] = RAW(row + 2, col);
        RAW(row, col) = median4(val);
      }
  }
}