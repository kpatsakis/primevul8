void AAHD::make_ahd_rb()
{
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_ahd_rb_hv(i);
  }
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_ahd_rb_last(i);
  }
}