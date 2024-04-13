void DHT::make_greens()
{
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_gline(i);
  }
}