void LibRaw::border_interpolate(int border)
{
  unsigned row, col, y, x, f, c, sum[8];

  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
    {
      if (col == border && row >= border && row < height - border)
        col = width - border;
      memset(sum, 0, sizeof sum);
      for (y = row - 1; y != row + 2; y++)
        for (x = col - 1; x != col + 2; x++)
          if (y < height && x < width)
          {
            f = fcol(y, x);
            sum[f] += image[y * width + x][f];
            sum[f + 4]++;
          }
      f = fcol(row, col);
      FORCC if (c != f && sum[c + 4]) image[row * width + col][c] =
          sum[c] / sum[c + 4];
    }
}