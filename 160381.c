void AAHD::refine_hv_dirs()
{
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    refine_hv_dirs(i, i & 1);
  }
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    refine_hv_dirs(i, (i & 1) ^ 1);
  }
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    refine_ihv_dirs(i);
  }
}