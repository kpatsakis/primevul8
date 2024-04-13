void LibRaw::x3f_dpq_interpolate_rg()
{
  int w = imgdata.sizes.raw_width / 2;
  int h = imgdata.sizes.raw_height / 2;
  unsigned short *image = (ushort *)imgdata.rawdata.color3_image;

  for (int color = 0; color < 2; color++)
  {
    for (int y = 2; y < (h - 2); y++)
    {
      uint16_t *row0 =
          &image[imgdata.sizes.raw_width * 3 * (y * 2) + color]; // dst[1]
      uint16_t row0_3 = row0[3];
      uint16_t *row1 =
          &image[imgdata.sizes.raw_width * 3 * (y * 2 + 1) + color]; // dst1[1]
      uint16_t row1_3 = row1[3];
      for (int x = 2; x < (w - 2); x++)
      {
        row1[0] = row1[3] = row0[3] = row0[0];
        row0 += 6;
        row1 += 6;
      }
    }
  }
}