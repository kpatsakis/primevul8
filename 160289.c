void AAHD::refine_hv_dirs(int i, int js)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int moff = nr_offset(i + nr_margin, nr_margin + js);
  for (int j = js; j < iwidth; j += 2, moff += 2)
  {
    int nv = (ndir[moff + Pn] & VER) + (ndir[moff + Ps] & VER) +
             (ndir[moff + Pw] & VER) + (ndir[moff + Pe] & VER);
    int nh = (ndir[moff + Pn] & HOR) + (ndir[moff + Ps] & HOR) +
             (ndir[moff + Pw] & HOR) + (ndir[moff + Pe] & HOR);
    bool codir = (ndir[moff] & VER)
                     ? ((ndir[moff + Pn] & VER) || (ndir[moff + Ps] & VER))
                     : ((ndir[moff + Pw] & HOR) || (ndir[moff + Pe] & HOR));
    nv /= VER;
    nh /= HOR;
    if ((ndir[moff] & VER) && (nh > 2 && !codir))
    {
      ndir[moff] &= ~VER;
      ndir[moff] |= HOR;
    }
    if ((ndir[moff] & HOR) && (nv > 2 && !codir))
    {
      ndir[moff] &= ~HOR;
      ndir[moff] |= VER;
    }
  }
}