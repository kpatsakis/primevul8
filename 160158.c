void DHT::make_rbdiag(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = libraw.COLOR(i, 0) & 1;
  int uc = libraw.COLOR(i, js);
  int cl = uc ^ 2;
  /*
   * js -- начальная х-координата, которая попадает на уже интерполированный
   * зелёный al -- известный цвет (кроме зелёного) в точке интерполирования cl
   * -- неизвестный цвет
   */
  for (int j = js; j < iwidth; j += 2)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    int dx, dy, dx2, dy2;
    if (ndir[nr_offset(y, x)] & LURD)
    {
      dx = -1;
      dx2 = 1;
      dy = -1;
      dy2 = 1;
    }
    else
    {
      dx = -1;
      dx2 = 1;
      dy = 1;
      dy2 = -1;
    }
    float g1 = 1 / calc_dist(nraw[nr_offset(y, x)][1],
                             nraw[nr_offset(y + dy, x + dx)][1]);
    float g2 = 1 / calc_dist(nraw[nr_offset(y, x)][1],
                             nraw[nr_offset(y + dy2, x + dx2)][1]);
    g1 *= g1 * g1;
    g2 *= g2 * g2;

    float eg;
    eg = nraw[nr_offset(y, x)][1] *
         (g1 * nraw[nr_offset(y + dy, x + dx)][cl] /
              nraw[nr_offset(y + dy, x + dx)][1] +
          g2 * nraw[nr_offset(y + dy2, x + dx2)][cl] /
              nraw[nr_offset(y + dy2, x + dx2)][1]) /
         (g1 + g2);
    float min, max;
    min = MIN(nraw[nr_offset(y + dy, x + dx)][cl],
              nraw[nr_offset(y + dy2, x + dx2)][cl]);
    max = MAX(nraw[nr_offset(y + dy, x + dx)][cl],
              nraw[nr_offset(y + dy2, x + dx2)][cl]);
    min /= 1.2;
    max *= 1.2;
    if (eg < min)
      eg = scale_under(eg, min);
    else if (eg > max)
      eg = scale_over(eg, max);
    if (eg > channel_maximum[cl])
      eg = channel_maximum[cl];
    else if (eg < channel_minimum[cl])
      eg = channel_minimum[cl];
    nraw[nr_offset(y, x)][cl] = eg;
  }
}