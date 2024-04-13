void LibRaw::x3f_dpq_interpolate_af_sd(int xstart, int ystart, int xend,
                                       int yend, int xstep, int ystep,
                                       int scale)
{
  unsigned short *image = (ushort *)imgdata.rawdata.color3_image;
  unsigned int rowpitch =
      imgdata.rawdata.sizes.raw_pitch / 2; // in 16-bit words
  // Interpolate single pixel
  for (int y = ystart; y < yend && y < imgdata.rawdata.sizes.height +
                                           imgdata.rawdata.sizes.top_margin;
       y += ystep)
  {
    uint16_t *row0 = &image[imgdata.sizes.raw_width * 3 * y]; // Наша строка
    uint16_t *row1 =
        &image[imgdata.sizes.raw_width * 3 * (y + 1)]; // Следующая строка
    uint16_t *row_minus =
        &image[imgdata.sizes.raw_width * 3 * (y - scale)]; // Строка выше
    uint16_t *row_plus =
        &image[imgdata.sizes.raw_width * 3 *
               (y + scale)]; // Строка ниже AF-point (scale=2 -> ниже row1
    uint16_t *row_minus1 = &image[imgdata.sizes.raw_width * 3 * (y - 1)];
    for (int x = xstart; x < xend && x < imgdata.rawdata.sizes.width +
                                             imgdata.rawdata.sizes.left_margin;
         x += xstep)
    {
      uint16_t *pixel00 = &row0[x * 3]; // Current pixel
      float sumR = 0.f, sumG = 0.f;
      float cnt = 0.f;
      for (int xx = -scale; xx <= scale; xx += scale)
      {
        sumR += row_minus[(x + xx) * 3];
        sumR += row_plus[(x + xx) * 3];
        sumG += row_minus[(x + xx) * 3 + 1];
        sumG += row_plus[(x + xx) * 3 + 1];
        cnt += 1.f;
        if (xx)
        {
          cnt += 1.f;
          sumR += row0[(x + xx) * 3];
          sumG += row0[(x + xx) * 3 + 1];
        }
      }
      pixel00[0] = sumR / 8.f;
      pixel00[1] = sumG / 8.f;

      if (scale == 2)
      {
        uint16_t *pixel0B = &row0[x * 3 + 3]; // right pixel
        uint16_t *pixel1B = &row1[x * 3 + 3]; // right pixel
        float sumG0 = 0, sumG1 = 0.f;
        float cnt = 0.f;
        for (int xx = -scale; xx <= scale; xx += scale)
        {
          sumG0 += row_minus1[(x + xx) * 3 + 2];
          sumG1 += row_plus[(x + xx) * 3 + 2];
          cnt += 1.f;
          if (xx)
          {
            sumG0 += row0[(x + xx) * 3 + 2];
            sumG1 += row1[(x + xx) * 3 + 2];
            cnt += 1.f;
          }
        }
        pixel0B[2] = sumG0 / cnt;
        pixel1B[2] = sumG1 / cnt;
      }

      //			uint16_t* pixel10 = &row1[x*3]; // Pixel below current
      //			uint16_t* pixel_bottom = &row_plus[x*3];
    }
  }
}