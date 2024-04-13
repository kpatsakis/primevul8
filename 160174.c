void LibRaw::copy_fuji_uncropped(unsigned short cblack[4],
                                 unsigned short *dmaxp)
{
  int row;
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
  for (row = 0; row < S.raw_height - S.top_margin * 2; row++)
  {
    int col;
    unsigned short ldmax = 0;
    for (col = 0;
         col < IO.fuji_width << !libraw_internal_data.unpacker_data.fuji_layout;
         col++)
    {
      unsigned r, c;
      if (libraw_internal_data.unpacker_data.fuji_layout)
      {
        r = IO.fuji_width - 1 - col + (row >> 1);
        c = col + ((row + 1) >> 1);
      }
      else
      {
        r = IO.fuji_width - 1 + row - (col >> 1);
        c = row + ((col + 1) >> 1);
      }
      if (r < S.height && c < S.width)
      {
        unsigned short val =
            imgdata.rawdata.raw_image[(row + S.top_margin) * S.raw_pitch / 2 +
                                      (col + S.left_margin)];
        int cc = FC(r, c);
        if (val > cblack[cc])
        {
          val -= cblack[cc];
          if (val > ldmax)
            ldmax = val;
        }
        else
          val = 0;
        imgdata.image[((r) >> IO.shrink) * S.iwidth + ((c) >> IO.shrink)][cc] =
            val;
      }
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