void AAHD::make_ahd_rb_last(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = libraw.COLOR(i, 0) & 1;
  int kc = libraw.COLOR(i, js);
  /*
   * js -- начальная х-координата, которая попадает мимо известного зелёного
   * kc -- известный цвет в точке интерполирования
   */
  int dirs[2][3] = {{Pnw, Pn, Pne}, {Pnw, Pw, Psw}};
  int moff = nr_offset(i + nr_margin, nr_margin);
  for (int j = 0; j < iwidth; j++)
  {
    for (int d = 0; d < 2; ++d)
    {
      ushort3 *cnr;
      cnr = &rgb_ahd[d][moff + j];
      int c = kc ^ 2;
      if ((j & 1) != js)
      {
        // точка зелёного, для вертикального направления нужен альтернативный
        // строчному цвет
        c ^= d << 1;
      }
      int bh, bk;
      int bgd = 0;
      for (int k = 0; k < 3; ++k)
        for (int h = 0; h < 3; ++h)
        {
          // градиент зелёного плюс градиент {r,b}
          int gd =
              ABS(2 * cnr[0][1] - (cnr[+dirs[d][k]][1] + cnr[-dirs[d][h]][1])) +
              ABS(cnr[+dirs[d][k]][c] - cnr[-dirs[d][h]][c]) / 4 +
              ABS(cnr[+dirs[d][k]][c] - cnr[+dirs[d][k]][1] +
                  cnr[-dirs[d][h]][1] - cnr[-dirs[d][h]][c]) /
                  4;
          if (bgd == 0 || gd < bgd)
          {
            bgd = gd;
            bh = h;
            bk = k;
          }
        }
      int h1 = cnr[+dirs[d][bk]][c] - cnr[+dirs[d][bk]][1];
      int h2 = cnr[-dirs[d][bh]][c] - cnr[-dirs[d][bh]][1];
      int eg = cnr[0][1] + (h1 + h2) / 2;
      //			int min = MIN(cnr[+dirs[d][bk]][c], cnr[-dirs[d][bh]][c]);
      //			int max = MAX(cnr[+dirs[d][bk]][c], cnr[-dirs[d][bh]][c]);
      //			min -= min / OverFraction;
      //			max += max / OverFraction;
      //			if (eg < min)
      //				eg = min - sqrt(min - eg);
      //			else if (eg > max)
      //				eg = max + sqrt(eg - max);
      if (eg > channel_maximum[c])
        eg = channel_maximum[c];
      else if (eg < channel_minimum[c])
        eg = channel_minimum[c];
      cnr[0][c] = eg;
    }
  }
}