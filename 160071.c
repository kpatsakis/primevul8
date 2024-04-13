void AAHD::refine_ihv_dirs(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int moff = nr_offset(i + nr_margin, nr_margin);
  for (int j = 0; j < iwidth; j++, ++moff)
  {
    if (ndir[moff] & HVSH)
      continue;
    int nv = (ndir[moff + Pn] & VER) + (ndir[moff + Ps] & VER) +
             (ndir[moff + Pw] & VER) + (ndir[moff + Pe] & VER);
    int nh = (ndir[moff + Pn] & HOR) + (ndir[moff + Ps] & HOR) +
             (ndir[moff + Pw] & HOR) + (ndir[moff + Pe] & HOR);
    nv /= VER;
    nh /= HOR;
    if ((ndir[moff] & VER) && nh > 3)
    {
      ndir[moff] &= ~VER;
      ndir[moff] |= HOR;
    }
    if ((ndir[moff] & HOR) && nv > 3)
    {
      ndir[moff] &= ~HOR;
      ndir[moff] |= VER;
    }
  }
}