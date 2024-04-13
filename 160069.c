void DHT::refine_ihv_dirs(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  for (int j = 0; j < iwidth; j++)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    if (ndir[nr_offset(y, x)] & HVSH)
      continue;
    int nv =
        (ndir[nr_offset(y - 1, x)] & VER) + (ndir[nr_offset(y + 1, x)] & VER) +
        (ndir[nr_offset(y, x - 1)] & VER) + (ndir[nr_offset(y, x + 1)] & VER);
    int nh =
        (ndir[nr_offset(y - 1, x)] & HOR) + (ndir[nr_offset(y + 1, x)] & HOR) +
        (ndir[nr_offset(y, x - 1)] & HOR) + (ndir[nr_offset(y, x + 1)] & HOR);
    nv /= VER;
    nh /= HOR;
    if ((ndir[nr_offset(y, x)] & VER) && nh > 3)
    {
      ndir[nr_offset(y, x)] &= ~VER;
      ndir[nr_offset(y, x)] |= HOR;
    }
    if ((ndir[nr_offset(y, x)] & HOR) && nv > 3)
    {
      ndir[nr_offset(y, x)] &= ~HOR;
      ndir[nr_offset(y, x)] |= VER;
    }
  }
}