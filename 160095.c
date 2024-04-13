void LibRaw::crxLoadDecodeLoop(void *img, int nPlanes)
{
#ifdef LIBRAW_USE_OPENMP
  int results[4]; // nPlanes is always <= 4
#pragma omp parallel for
  for (int32_t plane = 0; plane < nPlanes; ++plane)
    results[i] = crxDecodePlane(img, plane);

  for (int32_t plane = 0; plane < nPlanes; ++plane)
    if (results[i])
      derror();
#else
  for (int32_t plane = 0; plane < nPlanes; ++plane)
    if (crxDecodePlane(img, plane))
      derror();
#endif
}