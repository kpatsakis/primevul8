void LibRaw::convertFloatToInt(float dmin /* =4096.f */,
                               float dmax /* =32767.f */,
                               float dtarget /*= 16383.f */)
{
  int samples = 0;
  float *data = 0;
  if (imgdata.rawdata.float_image)
  {
    samples = 1;
    data = imgdata.rawdata.float_image;
  }
  else if (imgdata.rawdata.float3_image)
  {
    samples = 3;
    data = (float *)imgdata.rawdata.float3_image;
  }
  else if (imgdata.rawdata.float4_image)
  {
    samples = 4;
    data = (float *)imgdata.rawdata.float4_image;
  }
  else
    return;

  ushort *raw_alloc = (ushort *)malloc(
      imgdata.sizes.raw_height * imgdata.sizes.raw_width *
      libraw_internal_data.unpacker_data.tiff_samples * sizeof(ushort));
  float tmax = MAX(imgdata.color.maximum, 1);
  float datamax = imgdata.color.fmaximum;

  tmax = MAX(tmax, datamax);
  tmax = MAX(tmax, 1.f);

  float multip = 1.f;
  if (tmax < dmin || tmax > dmax)
  {
    imgdata.rawdata.color.fnorm = imgdata.color.fnorm = multip = dtarget / tmax;
    imgdata.rawdata.color.maximum = imgdata.color.maximum = dtarget;
    imgdata.rawdata.color.black = imgdata.color.black =
        (float)imgdata.color.black * multip;
    for (int i = 0;
         i < sizeof(imgdata.color.cblack) / sizeof(imgdata.color.cblack[0]);
         i++)
      if (i != 4 && i != 5)
        imgdata.rawdata.color.cblack[i] = imgdata.color.cblack[i] =
            (float)imgdata.color.cblack[i] * multip;
  }
  else
    imgdata.rawdata.color.fnorm = imgdata.color.fnorm = 0.f;

  for (size_t i = 0; i < imgdata.sizes.raw_height * imgdata.sizes.raw_width *
                             libraw_internal_data.unpacker_data.tiff_samples;
       ++i)
  {
    float val = MAX(data[i], 0.f);
    raw_alloc[i] = (ushort)(val * multip);
  }

  if (samples == 1)
  {
    imgdata.rawdata.raw_alloc = imgdata.rawdata.raw_image = raw_alloc;
    imgdata.rawdata.sizes.raw_pitch = imgdata.sizes.raw_pitch =
        imgdata.sizes.raw_width * 2;
  }
  else if (samples == 3)
  {
    imgdata.rawdata.raw_alloc = imgdata.rawdata.color3_image =
        (ushort(*)[3])raw_alloc;
    imgdata.rawdata.sizes.raw_pitch = imgdata.sizes.raw_pitch =
        imgdata.sizes.raw_width * 6;
  }
  else if (samples == 4)
  {
    imgdata.rawdata.raw_alloc = imgdata.rawdata.color4_image =
        (ushort(*)[4])raw_alloc;
    imgdata.rawdata.sizes.raw_pitch = imgdata.sizes.raw_pitch =
        imgdata.sizes.raw_width * 8;
  }
  free(data); // remove old allocation
  imgdata.rawdata.float_image = 0;
  imgdata.rawdata.float3_image = 0;
  imgdata.rawdata.float4_image = 0;
}