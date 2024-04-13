void LibRaw::ahd_interpolate_combine_homogeneous_pixels(
    int top, int left, ushort (*rgb)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3],
    char (*homogeneity_map)[LIBRAW_AHD_TILE][2])
{
  int row, col;
  int tr, tc;
  int i, j;
  int direction;
  int hm[2];
  int c;
  const int rowlimit = MIN(top + LIBRAW_AHD_TILE - 3, height - 5);
  const int collimit = MIN(left + LIBRAW_AHD_TILE - 3, width - 5);

  ushort(*pix)[4];
  ushort(*rix[2])[3];

  for (row = top + 3; row < rowlimit; row++)
  {
    tr = row - top;
    pix = &image[row * width + left + 2];
    for (direction = 0; direction < 2; direction++)
    {
      rix[direction] = &rgb[direction][tr][2];
    }

    for (col = left + 3; col < collimit; col++)
    {
      tc = col - left;
      pix++;
      for (direction = 0; direction < 2; direction++)
      {
        rix[direction]++;
      }

      for (direction = 0; direction < 2; direction++)
      {
        hm[direction] = 0;
        for (i = tr - 1; i <= tr + 1; i++)
        {
          for (j = tc - 1; j <= tc + 1; j++)
          {
            hm[direction] += homogeneity_map[i][j][direction];
          }
        }
      }
      if (hm[0] != hm[1])
      {
        memcpy(pix[0], rix[hm[1] > hm[0]][0], 3 * sizeof(ushort));
      }
      else
      {
        FORC3 { pix[0][c] = (rix[0][0][c] + rix[1][0][c]) >> 1; }
      }
    }
  }
}