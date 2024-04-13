void LibRaw::x3f_dpq_interpolate_af(int xstep, int ystep, int scale)
{
  unsigned short *image = (ushort *)imgdata.rawdata.color3_image;
  unsigned int rowpitch =
      imgdata.rawdata.sizes.raw_pitch / 2; // in 16-bit words
                                           // Interpolate single pixel
  for (int y = 0;
       y < imgdata.rawdata.sizes.height + imgdata.rawdata.sizes.top_margin;
       y += ystep)
  {
    if (y < imgdata.rawdata.sizes.top_margin)
      continue;
    if (y < scale)
      continue;
    if (y > imgdata.rawdata.sizes.raw_height - scale)
      break;
    uint16_t *row0 = &image[imgdata.sizes.raw_width * 3 * y]; // Наша строка
    uint16_t *row_minus =
        &image[imgdata.sizes.raw_width * 3 * (y - scale)]; // Строка выше
    uint16_t *row_plus =
        &image[imgdata.sizes.raw_width * 3 * (y + scale)]; // Строка ниже
    for (int x = 0;
         x < imgdata.rawdata.sizes.width + imgdata.rawdata.sizes.left_margin;
         x += xstep)
    {
      if (x < imgdata.rawdata.sizes.left_margin)
        continue;
      if (x < scale)
        continue;
      if (x > imgdata.rawdata.sizes.raw_width - scale)
        break;
      uint16_t *pixel0 = &row0[x * 3];
      uint16_t *pixel_top = &row_minus[x * 3];
      uint16_t *pixel_bottom = &row_plus[x * 3];
      uint16_t *pixel_left = &row0[(x - scale) * 3];
      uint16_t *pixel_right = &row0[(x + scale) * 3];
      uint16_t *pixf = pixel_top;
      if (_ABS(pixf[2] - pixel0[2]) > _ABS(pixel_bottom[2] - pixel0[2]))
        pixf = pixel_bottom;
      if (_ABS(pixf[2] - pixel0[2]) > _ABS(pixel_left[2] - pixel0[2]))
        pixf = pixel_left;
      if (_ABS(pixf[2] - pixel0[2]) > _ABS(pixel_right[2] - pixel0[2]))
        pixf = pixel_right;
      int blocal = pixel0[2], bnear = pixf[2];
      if (blocal < imgdata.color.black + 16 || bnear < imgdata.color.black + 16)
      {
        if (pixel0[0] < imgdata.color.black)
          pixel0[0] = imgdata.color.black;
        if (pixel0[1] < imgdata.color.black)
          pixel0[1] = imgdata.color.black;
        pixel0[0] = CLIP(
            (pixel0[0] - imgdata.color.black) * 4 + imgdata.color.black, 16383);
        pixel0[1] = CLIP(
            (pixel0[1] - imgdata.color.black) * 4 + imgdata.color.black, 16383);
      }
      else
      {
        float multip = float(bnear - imgdata.color.black) /
                       float(blocal - imgdata.color.black);
        if (pixel0[0] < imgdata.color.black)
          pixel0[0] = imgdata.color.black;
        if (pixel0[1] < imgdata.color.black)
          pixel0[1] = imgdata.color.black;
        float pixf0 = pixf[0];
        if (pixf0 < imgdata.color.black)
          pixf0 = imgdata.color.black;
        float pixf1 = pixf[1];
        if (pixf1 < imgdata.color.black)
          pixf1 = imgdata.color.black;

        pixel0[0] = CLIP(
            ((float(pixf0 - imgdata.color.black) * multip +
              imgdata.color.black) +
             ((pixel0[0] - imgdata.color.black) * 3.75 + imgdata.color.black)) /
                2,
            16383);
        pixel0[1] = CLIP(
            ((float(pixf1 - imgdata.color.black) * multip +
              imgdata.color.black) +
             ((pixel0[1] - imgdata.color.black) * 3.75 + imgdata.color.black)) /
                2,
            16383);
        // pixel0[1] = float(pixf[1]-imgdata.color.black)*multip +
        // imgdata.color.black;
      }
    }
  }
}