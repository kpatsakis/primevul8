void DHT::hide_hots()
{
  int iwidth = libraw.imgdata.sizes.iwidth;
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided) firstprivate(iwidth)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
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
      float c = nraw[nr_offset(y, x)][kc];
      if ((c > nraw[nr_offset(y, x + 2)][kc] &&
           c > nraw[nr_offset(y, x - 2)][kc] &&
           c > nraw[nr_offset(y - 2, x)][kc] &&
           c > nraw[nr_offset(y + 2, x)][kc] &&
           c > nraw[nr_offset(y, x + 1)][1] &&
           c > nraw[nr_offset(y, x - 1)][1] &&
           c > nraw[nr_offset(y - 1, x)][1] &&
           c > nraw[nr_offset(y + 1, x)][1]) ||
          (c < nraw[nr_offset(y, x + 2)][kc] &&
           c < nraw[nr_offset(y, x - 2)][kc] &&
           c < nraw[nr_offset(y - 2, x)][kc] &&
           c < nraw[nr_offset(y + 2, x)][kc] &&
           c < nraw[nr_offset(y, x + 1)][1] &&
           c < nraw[nr_offset(y, x - 1)][1] &&
           c < nraw[nr_offset(y - 1, x)][1] &&
           c < nraw[nr_offset(y + 1, x)][1]))
      {
        float avg = 0;
        for (int k = -2; k < 3; k += 2)
          for (int m = -2; m < 3; m += 2)
            if (m == 0 && k == 0)
              continue;
            else
              avg += nraw[nr_offset(y + k, x + m)][kc];
        avg /= 8;
        //				float dev = 0;
        //				for (int k = -2; k < 3; k += 2)
        //					for (int l = -2; l < 3; l += 2)
        //						if (k == 0 && l == 0)
        //							continue;
        //						else {
        //							float t = nraw[nr_offset(y + k, x + l)][kc] -
        //avg; 							dev += t * t;
        //						}
        //				dev /= 8;
        //				dev = sqrt(dev);
        if (calc_dist(c, avg) > Thot())
        {
          ndir[nr_offset(y, x)] |= HOT;
          float dv = calc_dist(
              nraw[nr_offset(y - 2, x)][kc] * nraw[nr_offset(y - 1, x)][1],
              nraw[nr_offset(y + 2, x)][kc] * nraw[nr_offset(y + 1, x)][1]);
          float dh = calc_dist(
              nraw[nr_offset(y, x - 2)][kc] * nraw[nr_offset(y, x - 1)][1],
              nraw[nr_offset(y, x + 2)][kc] * nraw[nr_offset(y, x + 1)][1]);
          if (dv > dh)
            nraw[nr_offset(y, x)][kc] = (nraw[nr_offset(y, x + 2)][kc] +
                                         nraw[nr_offset(y, x - 2)][kc]) /
                                        2;
          else
            nraw[nr_offset(y, x)][kc] = (nraw[nr_offset(y - 2, x)][kc] +
                                         nraw[nr_offset(y + 2, x)][kc]) /
                                        2;
        }
      }
    }
    for (int j = js ^ 1; j < iwidth; j += 2)
    {
      int x = j + nr_leftmargin;
      int y = i + nr_topmargin;
      float c = nraw[nr_offset(y, x)][1];
      if ((c > nraw[nr_offset(y, x + 2)][1] &&
           c > nraw[nr_offset(y, x - 2)][1] &&
           c > nraw[nr_offset(y - 2, x)][1] &&
           c > nraw[nr_offset(y + 2, x)][1] &&
           c > nraw[nr_offset(y, x + 1)][kc] &&
           c > nraw[nr_offset(y, x - 1)][kc] &&
           c > nraw[nr_offset(y - 1, x)][kc ^ 2] &&
           c > nraw[nr_offset(y + 1, x)][kc ^ 2]) ||
          (c < nraw[nr_offset(y, x + 2)][1] &&
           c < nraw[nr_offset(y, x - 2)][1] &&
           c < nraw[nr_offset(y - 2, x)][1] &&
           c < nraw[nr_offset(y + 2, x)][1] &&
           c < nraw[nr_offset(y, x + 1)][kc] &&
           c < nraw[nr_offset(y, x - 1)][kc] &&
           c < nraw[nr_offset(y - 1, x)][kc ^ 2] &&
           c < nraw[nr_offset(y + 1, x)][kc ^ 2]))
      {
        float avg = 0;
        for (int k = -2; k < 3; k += 2)
          for (int m = -2; m < 3; m += 2)
            if (k == 0 && m == 0)
              continue;
            else
              avg += nraw[nr_offset(y + k, x + m)][1];
        avg /= 8;
        //				float dev = 0;
        //				for (int k = -2; k < 3; k += 2)
        //					for (int l = -2; l < 3; l += 2)
        //						if (k == 0 && l == 0)
        //							continue;
        //						else {
        //							float t = nraw[nr_offset(y + k, x + l)][1] -
        //avg; 							dev += t * t;
        //						}
        //				dev /= 8;
        //				dev = sqrt(dev);
        if (calc_dist(c, avg) > Thot())
        {
          ndir[nr_offset(y, x)] |= HOT;
          float dv = calc_dist(
              nraw[nr_offset(y - 2, x)][1] * nraw[nr_offset(y - 1, x)][kc ^ 2],
              nraw[nr_offset(y + 2, x)][1] * nraw[nr_offset(y + 1, x)][kc ^ 2]);
          float dh = calc_dist(
              nraw[nr_offset(y, x - 2)][1] * nraw[nr_offset(y, x - 1)][kc],
              nraw[nr_offset(y, x + 2)][1] * nraw[nr_offset(y, x + 1)][kc]);
          if (dv > dh)
            nraw[nr_offset(y, x)][1] =
                (nraw[nr_offset(y, x + 2)][1] + nraw[nr_offset(y, x - 2)][1]) /
                2;
          else
            nraw[nr_offset(y, x)][1] =
                (nraw[nr_offset(y - 2, x)][1] + nraw[nr_offset(y + 2, x)][1]) /
                2;
        }
      }
    }
  }
}