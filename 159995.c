void AAHD::combine_image()
{
  for (int i = 0, i_out = 0; i < libraw.imgdata.sizes.iheight; ++i)
  {
    int moff = nr_offset(i + nr_margin, nr_margin);
    for (int j = 0; j < libraw.imgdata.sizes.iwidth; j++, ++moff, ++i_out)
    {
      if (ndir[moff] & HOT)
      {
        int c = libraw.COLOR(i, j);
        rgb_ahd[1][moff][c] = rgb_ahd[0][moff][c] =
            libraw.imgdata.image[i_out][c];
      }
      if (ndir[moff] & VER)
      {
        libraw.imgdata.image[i_out][0] = rgb_ahd[1][moff][0];
        libraw.imgdata.image[i_out][3] = libraw.imgdata.image[i_out][1] =
            rgb_ahd[1][moff][1];
        libraw.imgdata.image[i_out][2] = rgb_ahd[1][moff][2];
      }
      else
      {
        libraw.imgdata.image[i_out][0] = rgb_ahd[0][moff][0];
        libraw.imgdata.image[i_out][3] = libraw.imgdata.image[i_out][1] =
            rgb_ahd[0][moff][1];
        libraw.imgdata.image[i_out][2] = rgb_ahd[0][moff][2];
      }
    }
  }
}