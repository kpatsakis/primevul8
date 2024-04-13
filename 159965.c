void LibRaw::kodak_rgb_load_raw()
{
  if (!image)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  short buf[768], *bp;
  int row, col, len, c, i, rgb[3], ret;
  ushort *ip = image[0];

  for (row = 0; row < height; row++)
  {
    checkCancel();
    for (col = 0; col < width; col += 256)
    {
      len = MIN(256, width - col);
      ret = kodak_65000_decode(buf, len * 3);
      memset(rgb, 0, sizeof rgb);
      for (bp = buf, i = 0; i < len; i++, ip += 4)
        if (load_flags == 12)
          FORC3 ip[c] = ret ? (*bp++) : (rgb[c] += *bp++);
        else
          FORC3 if ((ip[c] = ret ? (*bp++) : (rgb[c] += *bp++)) >> 12) derror();
    }
  }
}