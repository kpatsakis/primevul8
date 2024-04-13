void LibRaw::fuji_14bit_load_raw()
{
  const unsigned linelen = S.raw_width * 7 / 4;
  const unsigned pitch = S.raw_pitch ? S.raw_pitch / 2 : S.raw_width;
  unsigned char *buf = (unsigned char *)malloc(linelen);
  merror(buf, "fuji_14bit_load_raw()");

  for (int row = 0; row < S.raw_height; row++)
  {
    unsigned bytesread =
        libraw_internal_data.internal_data.input->read(buf, 1, linelen);
    unsigned short *dest = &imgdata.rawdata.raw_image[pitch * row];
    if (bytesread % 28)
    {
      swab32arr((unsigned *)buf, bytesread / 4);
      for (int sp = 0, dp = 0;
           dp < pitch - 3 && sp < linelen - 6 && sp < bytesread - 6;
           sp += 7, dp += 4)
        unpack7bytesto4x16(buf + sp, dest + dp);
    }
    else
      for (int sp = 0, dp = 0;
           dp < pitch - 15 && sp < linelen - 27 && sp < bytesread - 27;
           sp += 28, dp += 16)
        unpack28bytesto16x16ns(buf + sp, dest + dp);
  }
  free(buf);
}