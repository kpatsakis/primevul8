void DHT::make_gline(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = libraw.COLOR(i, 0) & 1;
  int kc = libraw.COLOR(i, js);
  /*
   * js -- начальная х-координата, которая попадает мимо известного зелёного
   * kc -- известный цвет в точке интерполирования
   */
  for (int j = js; j < iwidth; j += 2)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    int dx, dy, dx2, dy2;
    float h1, h2;
    if (ndir[nr_offset(y, x)] & VER)
    {
      dx = dx2 = 0;
      dy = -1;
      dy2 = 1;
      h1 = 2 * nraw[nr_offset(y - 1, x)][1] /
           (nraw[nr_offset(y - 2, x)][kc] + nraw[nr_offset(y, x)][kc]);
      h2 = 2 * nraw[nr_offset(y + 1, x)][1] /
           (nraw[nr_offset(y + 2, x)][kc] + nraw[nr_offset(y, x)][kc]);
    }
    else
    {
      dy = dy2 = 0;
      dx = 1;
      dx2 = -1;
      h1 = 2 * nraw[nr_offset(y, x + 1)][1] /
           (nraw[nr_offset(y, x + 2)][kc] + nraw[nr_offset(y, x)][kc]);
      h2 = 2 * nraw[nr_offset(y, x - 1)][1] /
           (nraw[nr_offset(y, x - 2)][kc] + nraw[nr_offset(y, x)][kc]);
    }
    float b1 = 1 / calc_dist(nraw[nr_offset(y, x)][kc],
                             nraw[nr_offset(y + dy * 2, x + dx * 2)][kc]);
    float b2 = 1 / calc_dist(nraw[nr_offset(y, x)][kc],
                             nraw[nr_offset(y + dy2 * 2, x + dx2 * 2)][kc]);
    b1 *= b1;
    b2 *= b2;
    float eg = nraw[nr_offset(y, x)][kc] * (b1 * h1 + b2 * h2) / (b1 + b2);
    float min, max;
    min = MIN(nraw[nr_offset(y + dy, x + dx)][1],
              nraw[nr_offset(y + dy2, x + dx2)][1]);
    max = MAX(nraw[nr_offset(y + dy, x + dx)][1],
              nraw[nr_offset(y + dy2, x + dx2)][1]);
    min /= 1.2;
    max *= 1.2;
    if (eg < min)
      eg = scale_under(eg, min);
    else if (eg > max)
      eg = scale_over(eg, max);
    if (eg > channel_maximum[1])
      eg = channel_maximum[1];
    else if (eg < channel_minimum[1])
      eg = channel_minimum[1];
    nraw[nr_offset(y, x)][1] = eg;
  }
}