void LibRaw::ahd_interpolate_build_homogeneity_map(
    int top, int left, short (*lab)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3],
    char (*out_homogeneity_map)[LIBRAW_AHD_TILE][2])
{
  int row, col;
  int tr, tc;
  int direction;
  int i;
  short(*lix)[3];
  short(*lixs[2])[3];
  short *adjacent_lix;
  unsigned ldiff[2][4], abdiff[2][4], leps, abeps;
  static const int dir[4] = {-1, 1, -LIBRAW_AHD_TILE, LIBRAW_AHD_TILE};
  const int rowlimit = MIN(top + LIBRAW_AHD_TILE - 2, height - 4);
  const int collimit = MIN(left + LIBRAW_AHD_TILE - 2, width - 4);
  int homogeneity;
  char(*homogeneity_map_p)[2];

  memset(out_homogeneity_map, 0, 2 * LIBRAW_AHD_TILE * LIBRAW_AHD_TILE);

  for (row = top + 2; row < rowlimit; row++)
  {
    tr = row - top;
    homogeneity_map_p = &out_homogeneity_map[tr][1];
    for (direction = 0; direction < 2; direction++)
    {
      lixs[direction] = &lab[direction][tr][1];
    }

    for (col = left + 2; col < collimit; col++)
    {
      tc = col - left;
      homogeneity_map_p++;

      for (direction = 0; direction < 2; direction++)
      {
        lix = ++lixs[direction];
        for (i = 0; i < 4; i++)
        {
          adjacent_lix = lix[dir[i]];
          ldiff[direction][i] = ABS(lix[0][0] - adjacent_lix[0]);
          abdiff[direction][i] = SQR(lix[0][1] - adjacent_lix[1]) +
                                 SQR(lix[0][2] - adjacent_lix[2]);
        }
      }
      leps = MIN(MAX(ldiff[0][0], ldiff[0][1]), MAX(ldiff[1][2], ldiff[1][3]));
      abeps =
          MIN(MAX(abdiff[0][0], abdiff[0][1]), MAX(abdiff[1][2], abdiff[1][3]));
      for (direction = 0; direction < 2; direction++)
      {
        homogeneity = 0;
        for (i = 0; i < 4; i++)
        {
          if (ldiff[direction][i] <= leps && abdiff[direction][i] <= abeps)
          {
            homogeneity++;
          }
        }
        homogeneity_map_p[0][direction] = homogeneity;
      }
    }
  }
}