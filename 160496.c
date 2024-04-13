AAHD::AAHD(LibRaw &_libraw) : libraw(_libraw)
{
  nr_height = libraw.imgdata.sizes.iheight + nr_margin * 2;
  nr_width = libraw.imgdata.sizes.iwidth + nr_margin * 2;
  rgb_ahd[0] = (ushort3 *)calloc(nr_height * nr_width,
                                 (sizeof(ushort3) * 2 + sizeof(int3) * 2 + 3));
  if (!rgb_ahd[0])
    throw LIBRAW_EXCEPTION_ALLOC;

  rgb_ahd[1] = rgb_ahd[0] + nr_height * nr_width;
  yuv[0] = (int3 *)(rgb_ahd[1] + nr_height * nr_width);
  yuv[1] = yuv[0] + nr_height * nr_width;
  ndir = (char *)(yuv[1] + nr_height * nr_width);
  homo[0] = ndir + nr_height * nr_width;
  homo[1] = homo[0] + nr_height * nr_width;
  channel_maximum[0] = channel_maximum[1] = channel_maximum[2] = 0;
  channel_minimum[0] = libraw.imgdata.image[0][0];
  channel_minimum[1] = libraw.imgdata.image[0][1];
  channel_minimum[2] = libraw.imgdata.image[0][2];
  int iwidth = libraw.imgdata.sizes.iwidth;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
    {
      yuv_cam[i][j] = 0;
      for (int k = 0; k < 3; ++k)
        yuv_cam[i][j] += yuv_coeff[i][k] * libraw.imgdata.color.rgb_cam[k][j];
    }
  if (gammaLUT[0] < -0.1f)
  {
    float r;
    for (int i = 0; i < 0x10000; i++)
    {
      r = (float)i / 0x10000;
      gammaLUT[i] =
          0x10000 * (r < 0.0181 ? 4.5f * r : 1.0993f * pow(r, 0.45f) - .0993f);
    }
  }
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    int col_cache[48];
    for (int j = 0; j < 48; ++j)
    {
      int c = libraw.COLOR(i, j);
      if (c == 3)
        c = 1;
      col_cache[j] = c;
    }
    int moff = nr_offset(i + nr_margin, nr_margin);
    for (int j = 0; j < iwidth; ++j, ++moff)
    {
      int c = col_cache[j % 48];
      unsigned short d = libraw.imgdata.image[i * iwidth + j][c];
      if (d != 0)
      {
        if (channel_maximum[c] < d)
          channel_maximum[c] = d;
        if (channel_minimum[c] > d)
          channel_minimum[c] = d;
        rgb_ahd[1][moff][c] = rgb_ahd[0][moff][c] = d;
      }
    }
  }
  channels_max =
      MAX(MAX(channel_maximum[0], channel_maximum[1]), channel_maximum[2]);
}