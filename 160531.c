void AAHD::illustrate_dline(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  for (int j = 0; j < iwidth; j++)
  {
    int x = j + nr_margin;
    int y = i + nr_margin;
    rgb_ahd[1][nr_offset(y, x)][0] = rgb_ahd[1][nr_offset(y, x)][1] =
        rgb_ahd[1][nr_offset(y, x)][2] = rgb_ahd[0][nr_offset(y, x)][0] =
            rgb_ahd[0][nr_offset(y, x)][1] = rgb_ahd[0][nr_offset(y, x)][2] = 0;
    int l = ndir[nr_offset(y, x)] & HVSH;
    l /= HVSH;
    if (ndir[nr_offset(y, x)] & VER)
      rgb_ahd[1][nr_offset(y, x)][0] =
          l * channel_maximum[0] / 4 + channel_maximum[0] / 4;
    else
      rgb_ahd[0][nr_offset(y, x)][2] =
          l * channel_maximum[2] / 4 + channel_maximum[2] / 4;
  }
}