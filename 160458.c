void DHT::make_rbhv(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = (libraw.COLOR(i, 0) & 1) ^ 1;
  for (int j = js; j < iwidth; j += 2)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    /*
     * поскольку сверху-снизу и справа-слева уже есть все необходимые красные и
     * синие, то можно выбрать наилучшее направление исходя из информации по
     * обоим цветам.
     */
    int dx, dy, dx2, dy2;
    float h1, h2;
    if (ndir[nr_offset(y, x)] & VER)
    {
      dx = dx2 = 0;
      dy = -1;
      dy2 = 1;
    }
    else
    {
      dy = dy2 = 0;
      dx = 1;
      dx2 = -1;
    }
    float g1 = 1 / calc_dist(nraw[nr_offset(y, x)][1],
                             nraw[nr_offset(y + dy, x + dx)][1]);
    float g2 = 1 / calc_dist(nraw[nr_offset(y, x)][1],
                             nraw[nr_offset(y + dy2, x + dx2)][1]);
    g1 *= g1;
    g2 *= g2;
    float eg_r, eg_b;
    eg_r = nraw[nr_offset(y, x)][1] *
           (g1 * nraw[nr_offset(y + dy, x + dx)][0] /
                nraw[nr_offset(y + dy, x + dx)][1] +
            g2 * nraw[nr_offset(y + dy2, x + dx2)][0] /
                nraw[nr_offset(y + dy2, x + dx2)][1]) /
           (g1 + g2);
    eg_b = nraw[nr_offset(y, x)][1] *
           (g1 * nraw[nr_offset(y + dy, x + dx)][2] /
                nraw[nr_offset(y + dy, x + dx)][1] +
            g2 * nraw[nr_offset(y + dy2, x + dx2)][2] /
                nraw[nr_offset(y + dy2, x + dx2)][1]) /
           (g1 + g2);
    float min_r, max_r;
    min_r = MIN(nraw[nr_offset(y + dy, x + dx)][0],
                nraw[nr_offset(y + dy2, x + dx2)][0]);
    max_r = MAX(nraw[nr_offset(y + dy, x + dx)][0],
                nraw[nr_offset(y + dy2, x + dx2)][0]);
    float min_b, max_b;
    min_b = MIN(nraw[nr_offset(y + dy, x + dx)][2],
                nraw[nr_offset(y + dy2, x + dx2)][2]);
    max_b = MAX(nraw[nr_offset(y + dy, x + dx)][2],
                nraw[nr_offset(y + dy2, x + dx2)][2]);
    min_r /= 1.2;
    max_r *= 1.2;
    min_b /= 1.2;
    max_b *= 1.2;

    if (eg_r < min_r)
      eg_r = scale_under(eg_r, min_r);
    else if (eg_r > max_r)
      eg_r = scale_over(eg_r, max_r);
    if (eg_b < min_b)
      eg_b = scale_under(eg_b, min_b);
    else if (eg_b > max_b)
      eg_b = scale_over(eg_b, max_b);

    if (eg_r > channel_maximum[0])
      eg_r = channel_maximum[0];
    else if (eg_r < channel_minimum[0])
      eg_r = channel_minimum[0];
    if (eg_b > channel_maximum[2])
      eg_b = channel_maximum[2];
    else if (eg_b < channel_minimum[2])
      eg_b = channel_minimum[2];
    nraw[nr_offset(y, x)][0] = eg_r;
    nraw[nr_offset(y, x)][2] = eg_b;
  }
}