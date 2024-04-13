void LibRaw::lin_interpolate()
{
  int code[16][16][32], size = 16, *ip, sum[4];
  int f, c, x, y, row, col, shift, color;

  RUN_CALLBACK(LIBRAW_PROGRESS_INTERPOLATE, 0, 3);

  if (filters == 9)
    size = 6;
  border_interpolate(1);
  for (row = 0; row < size; row++)
    for (col = 0; col < size; col++)
    {
      ip = code[row][col] + 1;
      f = fcol(row, col);
      memset(sum, 0, sizeof sum);
      for (y = -1; y <= 1; y++)
        for (x = -1; x <= 1; x++)
        {
          shift = (y == 0) + (x == 0);
          color = fcol(row + y, col + x);
          if (color == f)
            continue;
          *ip++ = (width * y + x) * 4 + color;
          *ip++ = shift;
          *ip++ = color;
          sum[color] += 1 << shift;
        }
      code[row][col][0] = (ip - code[row][col]) / 3;
      FORCC
      if (c != f)
      {
        *ip++ = c;
        *ip++ = sum[c] > 0 ? 256 / sum[c] : 0;
      }
    }
  RUN_CALLBACK(LIBRAW_PROGRESS_INTERPOLATE, 1, 3);
  lin_interpolate_loop(code, size);
  RUN_CALLBACK(LIBRAW_PROGRESS_INTERPOLATE, 2, 3);
}