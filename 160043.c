void DHT::restore_hots()
{
  int iwidth = libraw.imgdata.sizes.iwidth;
#if defined(LIBRAW_USE_OPENMP)
#ifdef _MSC_VER
#pragma omp parallel for firstprivate(iwidth)
#else
#pragma omp parallel for schedule(guided) firstprivate(iwidth) collapse(2)
#endif
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    for (int j = 0; j < iwidth; ++j)
    {
      int x = j + nr_leftmargin;
      int y = i + nr_topmargin;
      if (ndir[nr_offset(y, x)] & HOT)
      {
        int l = libraw.COLOR(i, j);
        nraw[nr_offset(i + nr_topmargin, j + nr_leftmargin)][l] =
            libraw.imgdata.image[i * iwidth + j][l];
      }
    }
  }
}