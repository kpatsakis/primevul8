void DHT::make_hv_dirs()
{
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    make_hv_dline(i);
  }
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    refine_hv_dirs(i, i & 1);
  }
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    refine_hv_dirs(i, (i & 1) ^ 1);
  }
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for schedule(guided)
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    refine_ihv_dirs(i);
  }
}