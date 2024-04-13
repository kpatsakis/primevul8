void DHT::make_rb()
{
#if defined(LIBRAW_USE_OPENMP)
#pragma omp barrier
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_rbdiag(i);
  }
#if defined(LIBRAW_USE_OPENMP)
#pragma omp barrier
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_rbhv(i);
  }
}