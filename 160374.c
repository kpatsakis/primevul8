void LibRaw::convert_to_rgb_loop(float out_cam[3][4])
{
  int row, col, c;
  float out[3];
  ushort *img;
  memset(libraw_internal_data.output_data.histogram, 0,
         sizeof(int) * LIBRAW_HISTOGRAM_SIZE * 4);
  for (img = imgdata.image[0], row = 0; row < S.height; row++)
    for (col = 0; col < S.width; col++, img += 4)
    {
      if (!libraw_internal_data.internal_output_params.raw_color)
      {
        out[0] = out[1] = out[2] = 0;
        for (c = 0; c < imgdata.idata.colors; c++)
        {
          out[0] += out_cam[0][c] * img[c];
          out[1] += out_cam[1][c] * img[c];
          out[2] += out_cam[2][c] * img[c];
        }
        for (c = 0; c < 3; c++)
          img[c] = CLIP((int)out[c]);
      }
      for (c = 0; c < imgdata.idata.colors; c++)
        libraw_internal_data.output_data.histogram[c][img[c] >> 3]++;
    }
}