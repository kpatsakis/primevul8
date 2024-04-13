void AAHD::hide_hots()
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    int js = libraw.COLOR(i, 0) & 1;
    int kc = libraw.COLOR(i, js);
    /*
     * js -- начальная х-координата, которая попадает мимо известного зелёного
     * kc -- известный цвет в точке интерполирования
     */
    int moff = nr_offset(i + nr_margin, nr_margin + js);
    for (int j = js; j < iwidth; j += 2, moff += 2)
    {
      ushort3 *rgb = &rgb_ahd[0][moff];
      int c = rgb[0][kc];
      if ((c > rgb[2 * Pe][kc] && c > rgb[2 * Pw][kc] && c > rgb[2 * Pn][kc] &&
           c > rgb[2 * Ps][kc] && c > rgb[Pe][1] && c > rgb[Pw][1] &&
           c > rgb[Pn][1] && c > rgb[Ps][1]) ||
          (c < rgb[2 * Pe][kc] && c < rgb[2 * Pw][kc] && c < rgb[2 * Pn][kc] &&
           c < rgb[2 * Ps][kc] && c < rgb[Pe][1] && c < rgb[Pw][1] &&
           c < rgb[Pn][1] && c < rgb[Ps][1]))
      {
        int chot = c >> Thot;
        int cdead = c << Tdead;
        int avg = 0;
        for (int k = -2; k < 3; k += 2)
          for (int m = -2; m < 3; m += 2)
            if (m == 0 && k == 0)
              continue;
            else
              avg += rgb[nr_offset(k, m)][kc];
        avg /= 8;
        if (chot > avg || cdead < avg)
        {
          ndir[moff] |= HOT;
          int dh =
              ABS(rgb[2 * Pw][kc] - rgb[2 * Pe][kc]) +
              ABS(rgb[Pw][1] - rgb[Pe][1]) +
              ABS(rgb[Pw][1] - rgb[Pe][1] + rgb[2 * Pe][kc] - rgb[2 * Pw][kc]);
          int dv =
              ABS(rgb[2 * Pn][kc] - rgb[2 * Ps][kc]) +
              ABS(rgb[Pn][1] - rgb[Ps][1]) +
              ABS(rgb[Pn][1] - rgb[Ps][1] + rgb[2 * Ps][kc] - rgb[2 * Pn][kc]);
          int d;
          if (dv > dh)
            d = Pw;
          else
            d = Pn;
          rgb_ahd[1][moff][kc] = rgb[0][kc] =
              (rgb[+2 * d][kc] + rgb[-2 * d][kc]) / 2;
        }
      }
    }
    js ^= 1;
    moff = nr_offset(i + nr_margin, nr_margin + js);
    for (int j = js; j < iwidth; j += 2, moff += 2)
    {
      ushort3 *rgb = &rgb_ahd[0][moff];
      int c = rgb[0][1];
      if ((c > rgb[2 * Pe][1] && c > rgb[2 * Pw][1] && c > rgb[2 * Pn][1] &&
           c > rgb[2 * Ps][1] && c > rgb[Pe][kc] && c > rgb[Pw][kc] &&
           c > rgb[Pn][kc ^ 2] && c > rgb[Ps][kc ^ 2]) ||
          (c < rgb[2 * Pe][1] && c < rgb[2 * Pw][1] && c < rgb[2 * Pn][1] &&
           c < rgb[2 * Ps][1] && c < rgb[Pe][kc] && c < rgb[Pw][kc] &&
           c < rgb[Pn][kc ^ 2] && c < rgb[Ps][kc ^ 2]))
      {
        int chot = c >> Thot;
        int cdead = c << Tdead;
        int avg = 0;
        for (int k = -2; k < 3; k += 2)
          for (int m = -2; m < 3; m += 2)
            if (k == 0 && m == 0)
              continue;
            else
              avg += rgb[nr_offset(k, m)][1];
        avg /= 8;
        if (chot > avg || cdead < avg)
        {
          ndir[moff] |= HOT;
          int dh =
              ABS(rgb[2 * Pw][1] - rgb[2 * Pe][1]) +
              ABS(rgb[Pw][kc] - rgb[Pe][kc]) +
              ABS(rgb[Pw][kc] - rgb[Pe][kc] + rgb[2 * Pe][1] - rgb[2 * Pw][1]);
          int dv = ABS(rgb[2 * Pn][1] - rgb[2 * Ps][1]) +
                   ABS(rgb[Pn][kc ^ 2] - rgb[Ps][kc ^ 2]) +
                   ABS(rgb[Pn][kc ^ 2] - rgb[Ps][kc ^ 2] + rgb[2 * Ps][1] -
                       rgb[2 * Pn][1]);
          int d;
          if (dv > dh)
            d = Pw;
          else
            d = Pn;
          rgb_ahd[1][moff][1] = rgb[0][1] =
              (rgb[+2 * d][1] + rgb[-2 * d][1]) / 2;
        }
      }
    }
  }
}