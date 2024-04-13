void AAHD::make_ahd_greens()
{
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_ahd_gline(i);
  }
}