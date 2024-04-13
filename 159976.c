void LibRaw::remove_zeroes()
{
  unsigned row, col, tot, n;
  int r, c;

  RUN_CALLBACK(LIBRAW_PROGRESS_REMOVE_ZEROES, 0, 2);

  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
      if (BAYER(row, col) == 0)
      {
        tot = n = 0;
        for (r = (int)row - 2; r <= (int)row + 2; r++)
          for (c = (int)col - 2; c <= (int)col + 2; c++)
            if (r >= 0 && r < height && c >= 0 && c < width &&
                FC(r, c) == FC(row, col) && BAYER(r, c))
              tot += (n++, BAYER(r, c));
        if (n)
          BAYER(row, col) = tot / n;
      }
  RUN_CALLBACK(LIBRAW_PROGRESS_REMOVE_ZEROES, 1, 2);
}