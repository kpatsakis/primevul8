void LibRaw::ahd_interpolate_green_h_and_v(
    int top, int left, ushort (*out_rgb)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3])
{
  int row, col;
  int c, val;
  ushort(*pix)[4];
  const int rowlimit = MIN(top + LIBRAW_AHD_TILE, height - 2);
  const int collimit = MIN(left + LIBRAW_AHD_TILE, width - 2);

  for (row = top; row < rowlimit; row++)
  {
    col = left + (FC(row, left) & 1);
    for (c = FC(row, col); col < collimit; col += 2)
    {
      pix = image + row * width + col;
      val =
          ((pix[-1][1] + pix[0][c] + pix[1][1]) * 2 - pix[-2][c] - pix[2][c]) >>
          2;
      out_rgb[0][row - top][col - left][1] = ULIM(val, pix[-1][1], pix[1][1]);
      val = ((pix[-width][1] + pix[0][c] + pix[width][1]) * 2 -
             pix[-2 * width][c] - pix[2 * width][c]) >>
            2;
      out_rgb[1][row - top][col - left][1] =
          ULIM(val, pix[-width][1], pix[width][1]);
    }
  }
}