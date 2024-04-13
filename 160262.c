void LibRaw::copy_bayer(unsigned short cblack[4], unsigned short *dmaxp)
{
  // Both cropped and uncropped
  int row;

#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
  for (row = 0; row < S.height && row + S.top_margin < S.raw_height; row++)
  {
    int col;
    unsigned short ldmax = 0;
    for (col = 0; col < S.width && col + S.left_margin < S.raw_width; col++)
    {
      unsigned short val =
          imgdata.rawdata.raw_image[(row + S.top_margin) * S.raw_pitch / 2 +
                                    (col + S.left_margin)];
      int cc = fcol(row, col);
      if (val > cblack[cc])
      {
        val -= cblack[cc];
        if (val > ldmax)
          ldmax = val;
      }
      else
        val = 0;
      imgdata
          .image[((row) >> IO.shrink) * S.iwidth + ((col) >> IO.shrink)][cc] =
          val;
    }
#if defined(LIBRAW_USE_OPENMP)
#pragma omp critical(dataupdate)
#endif
    {
      if (*dmaxp < ldmax)
        *dmaxp = ldmax;
    }
  }
}