void AAHD::illustrate_dirs()
{
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    illustrate_dline(i);
  }
}