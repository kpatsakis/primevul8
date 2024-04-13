void DHT::copy_to_image()
{
  int iwidth = libraw.imgdata.sizes.iwidth;
#if defined(LIBRAW_USE_OPENMP)
#ifdef _MSC_VER
#pragma omp parallel for
#else
#pragma omp parallel for schedule(guided) collapse(2)
#endif
#endif
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    for (int j = 0; j < iwidth; ++j)
    {
      libraw.imgdata.image[i * iwidth + j][0] =
          (unsigned short)(nraw[nr_offset(i + nr_topmargin, j + nr_leftmargin)]
                               [0]);
      libraw.imgdata.image[i * iwidth + j][2] =
          (unsigned short)(nraw[nr_offset(i + nr_topmargin, j + nr_leftmargin)]
                               [2]);
      libraw.imgdata.image[i * iwidth + j][1] =
          libraw.imgdata.image[i * iwidth + j][3] =
              (unsigned short)(nraw[nr_offset(i + nr_topmargin,
                                              j + nr_leftmargin)][1]);
    }
  }
}