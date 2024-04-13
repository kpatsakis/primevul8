void DHT::make_diag_dline(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = libraw.COLOR(i, 0) & 1;
  int kc = libraw.COLOR(i, js);
  /*
   * js -- начальная х-координата, которая попадает мимо известного зелёного
   * kc -- известный цвет в точке интерполирования
   */
  for (int j = 0; j < iwidth; j++)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    char d = 0;
    if ((j & 1) == js)
    {
      d = get_diag_grb(x, y, kc);
    }
    else
    {
      d = get_diag_rbg(x, y, kc);
    }
    ndir[nr_offset(y, x)] |= d;
  }
}