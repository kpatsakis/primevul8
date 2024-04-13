void DHT::refine_diag_dirs(int i, int js)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  for (int j = js; j < iwidth; j += 2)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    if (ndir[nr_offset(y, x)] & DIASH)
      continue;
    int nv = (ndir[nr_offset(y - 1, x)] & LURD) +
             (ndir[nr_offset(y + 1, x)] & LURD) +
             (ndir[nr_offset(y, x - 1)] & LURD) +
             (ndir[nr_offset(y, x + 1)] & LURD) +
             (ndir[nr_offset(y - 1, x - 1)] & LURD) +
             (ndir[nr_offset(y - 1, x + 1)] & LURD) +
             (ndir[nr_offset(y + 1, x - 1)] & LURD) +
             (ndir[nr_offset(y + 1, x + 1)] & LURD);
    int nh = (ndir[nr_offset(y - 1, x)] & RULD) +
             (ndir[nr_offset(y + 1, x)] & RULD) +
             (ndir[nr_offset(y, x - 1)] & RULD) +
             (ndir[nr_offset(y, x + 1)] & RULD) +
             (ndir[nr_offset(y - 1, x - 1)] & RULD) +
             (ndir[nr_offset(y - 1, x + 1)] & RULD) +
             (ndir[nr_offset(y + 1, x - 1)] & RULD) +
             (ndir[nr_offset(y + 1, x + 1)] & RULD);
    bool codir = (ndir[nr_offset(y, x)] & LURD)
                     ? ((ndir[nr_offset(y - 1, x - 1)] & LURD) ||
                        (ndir[nr_offset(y + 1, x + 1)] & LURD))
                     : ((ndir[nr_offset(y - 1, x + 1)] & RULD) ||
                        (ndir[nr_offset(y + 1, x - 1)] & RULD));
    nv /= LURD;
    nh /= RULD;
    if ((ndir[nr_offset(y, x)] & LURD) && (nh > 4 && !codir))
    {
      ndir[nr_offset(y, x)] &= ~LURD;
      ndir[nr_offset(y, x)] |= RULD;
    }
    if ((ndir[nr_offset(y, x)] & RULD) && (nv > 4 && !codir))
    {
      ndir[nr_offset(y, x)] &= ~RULD;
      ndir[nr_offset(y, x)] |= LURD;
    }
  }
}