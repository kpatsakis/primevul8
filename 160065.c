void LibRaw::ahd_interpolate_r_and_b_in_rgb_and_convert_to_cielab(
    int top, int left, ushort (*inout_rgb)[LIBRAW_AHD_TILE][3],
    short (*out_lab)[LIBRAW_AHD_TILE][3])
{
  unsigned row, col;
  int c, val;
  ushort(*pix)[4];
  ushort(*rix)[3];
  short(*lix)[3];
  float xyz[3];
  const unsigned num_pix_per_row = 4 * width;
  const unsigned rowlimit = MIN(top + LIBRAW_AHD_TILE - 1, height - 3);
  const unsigned collimit = MIN(left + LIBRAW_AHD_TILE - 1, width - 3);
  ushort *pix_above;
  ushort *pix_below;
  int t1, t2;

  for (row = top + 1; row < rowlimit; row++)
  {
    pix = image + row * width + left;
    rix = &inout_rgb[row - top][0];
    lix = &out_lab[row - top][0];

    for (col = left + 1; col < collimit; col++)
    {
      pix++;
      pix_above = &pix[0][0] - num_pix_per_row;
      pix_below = &pix[0][0] + num_pix_per_row;
      rix++;
      lix++;

      c = 2 - FC(row, col);

      if (c == 1)
      {
        c = FC(row + 1, col);
        t1 = 2 - c;
        val = pix[0][1] +
              ((pix[-1][t1] + pix[1][t1] - rix[-1][1] - rix[1][1]) >> 1);
        rix[0][t1] = CLIP(val);
        val =
            pix[0][1] + ((pix_above[c] + pix_below[c] -
                          rix[-LIBRAW_AHD_TILE][1] - rix[LIBRAW_AHD_TILE][1]) >>
                         1);
      }
      else
      {
        t1 = -4 + c; /* -4+c: pixel of color c to the left */
        t2 = 4 + c;  /* 4+c: pixel of color c to the right */
        val = rix[0][1] +
              ((pix_above[t1] + pix_above[t2] + pix_below[t1] + pix_below[t2] -
                rix[-LIBRAW_AHD_TILE - 1][1] - rix[-LIBRAW_AHD_TILE + 1][1] -
                rix[+LIBRAW_AHD_TILE - 1][1] - rix[+LIBRAW_AHD_TILE + 1][1] +
                1) >>
               2);
      }
      rix[0][c] = CLIP(val);
      c = FC(row, col);
      rix[0][c] = pix[0][c];
      cielab(rix[0], lix[0]);
    }
  }
}