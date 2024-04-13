DHT::DHT(LibRaw &_libraw) : libraw(_libraw)
{
  nr_height = libraw.imgdata.sizes.iheight + nr_topmargin * 2;
  nr_width = libraw.imgdata.sizes.iwidth + nr_leftmargin * 2;
  nraw = (float3 *)malloc(nr_height * nr_width * sizeof(float3));
  int iwidth = libraw.imgdata.sizes.iwidth;
  ndir = (char *)calloc(nr_height * nr_width, 1);
  channel_maximum[0] = channel_maximum[1] = channel_maximum[2] = 0;
  channel_minimum[0] = libraw.imgdata.image[0][0];
  channel_minimum[1] = libraw.imgdata.image[0][1];
  channel_minimum[2] = libraw.imgdata.image[0][2];
  for (int i = 0; i < nr_height * nr_width; ++i)
    nraw[i][0] = nraw[i][1] = nraw[i][2] = 0.5;
  for (int i = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    int col_cache[48];
    for (int j = 0; j < 48; ++j)
    {
      int l = libraw.COLOR(i, j);
      if (l == 3)
        l = 1;
      col_cache[j] = l;
    }
    for (int j = 0; j < iwidth; ++j)
    {
      int l = col_cache[j % 48];
      unsigned short c = libraw.imgdata.image[i * iwidth + j][l];
      if (c != 0)
      {
        if (channel_maximum[l] < c)
          channel_maximum[l] = c;
        if (channel_minimum[l] > c)
          channel_minimum[l] = c;
        nraw[nr_offset(i + nr_topmargin, j + nr_leftmargin)][l] = (float)c;
      }
    }
  }
  channel_minimum[0] += .5;
  channel_minimum[1] += .5;
  channel_minimum[2] += .5;
}