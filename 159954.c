void DHT::illustrate_dirs()
{
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    illustrate_dline(i);
  }
}