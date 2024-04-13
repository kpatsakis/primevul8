void DHT::illustrate_dline(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  for (int j = 0; j < iwidth; j++)
  {
    int x = j + nr_leftmargin;
    int y = i + nr_topmargin;
    nraw[nr_offset(y, x)][0] = nraw[nr_offset(y, x)][1] =
        nraw[nr_offset(y, x)][2] = 0.5;
    int l = ndir[nr_offset(y, x)] & 8;
    // l >>= 3; // WTF?
    l = 1;
    if (ndir[nr_offset(y, x)] & HOT)
      nraw[nr_offset(y, x)][0] =
          l * channel_maximum[0] / 4 + channel_maximum[0] / 4;
    else
      nraw[nr_offset(y, x)][2] =
          l * channel_maximum[2] / 4 + channel_maximum[2] / 4;
  }
}