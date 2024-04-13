void AAHD::make_ahd_gline(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = libraw.COLOR(i, 0) & 1;
  int kc = libraw.COLOR(i, js);
  /*
   * js -- начальная х-координата, которая попадает мимо известного зелёного
   * kc -- известный цвет в точке интерполирования
   */
  int hvdir[2] = {Pe, Ps};
  for (int d = 0; d < 2; ++d)
  {
    int moff = nr_offset(i + nr_margin, nr_margin + js);
    for (int j = js; j < iwidth; j += 2, moff += 2)
    {
      ushort3 *cnr;
      cnr = &rgb_ahd[d][moff];
      int h1 = 2 * cnr[-hvdir[d]][1] - int(cnr[-2 * hvdir[d]][kc] + cnr[0][kc]);
      int h2 = 2 * cnr[+hvdir[d]][1] - int(cnr[+2 * hvdir[d]][kc] + cnr[0][kc]);
      int h0 = (h1 + h2) / 4;
      int eg = cnr[0][kc] + h0;
      int min = MIN(cnr[-hvdir[d]][1], cnr[+hvdir[d]][1]);
      int max = MAX(cnr[-hvdir[d]][1], cnr[+hvdir[d]][1]);
      min -= min / OverFraction;
      max += max / OverFraction;
      if (eg < min)
        eg = min - sqrt(float(min - eg));
      else if (eg > max)
        eg = max + sqrt(float(eg - max));
      if (eg > channel_maximum[1])
        eg = channel_maximum[1];
      else if (eg < channel_minimum[1])
        eg = channel_minimum[1];
      cnr[0][1] = eg;
    }
  }
}