void LibRaw::nikon_coolscan_load_raw()
{
  if (!image)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;

  int bypp = tiff_bps <= 8 ? 1 : 2;
  int bufsize = width * 3 * bypp;
  unsigned char *buf = (unsigned char *)malloc(bufsize);
  unsigned short *ubuf = (unsigned short *)buf;

  if (tiff_bps <= 8)
    gamma_curve(1.0 / imgdata.params.coolscan_nef_gamma, 0., 1, 255);
  else
    gamma_curve(1.0 / imgdata.params.coolscan_nef_gamma, 0., 1, 65535);
  fseek(ifp, data_offset, SEEK_SET);
  for (int row = 0; row < raw_height; row++)
  {
      if(tiff_bps <=8)
        fread(buf, 1, bufsize, ifp);
      else
          read_shorts(ubuf,width*3);
    unsigned short(*ip)[4] = (unsigned short(*)[4])image + row * width;
    if (is_NikonTransfer == 2)
    { // it is also (tiff_bps == 8)
      for (int col = 0; col < width; col++)
      {
        ip[col][0] = ((float)curve[buf[col * 3]]) / 255.0f;
        ip[col][1] = ((float)curve[buf[col * 3 + 1]]) / 255.0f;
        ip[col][2] = ((float)curve[buf[col * 3 + 2]]) / 255.0f;
        ip[col][3] = 0;
      }
    }
    else if (tiff_bps <= 8)
    {
      for (int col = 0; col < width; col++)
      {
        ip[col][0] = curve[buf[col * 3]];
        ip[col][1] = curve[buf[col * 3 + 1]];
        ip[col][2] = curve[buf[col * 3 + 2]];
        ip[col][3] = 0;
      }
    }
    else
    {
      for (int col = 0; col < width; col++)
      {
        ip[col][0] = curve[ubuf[col * 3]];
        ip[col][1] = curve[ubuf[col * 3 + 1]];
        ip[col][2] = curve[ubuf[col * 3 + 2]];
        ip[col][3] = 0;
      }
    }
  }
  free(buf);
}