int LibRaw::kodak_65000_decode(short *out, int bsize)
{
  uchar c, blen[768];
  ushort raw[6];
  INT64 bitbuf = 0;
  int save, bits = 0, i, j, len, diff;

  save = ftell(ifp);
  bsize = (bsize + 3) & -4;
  for (i = 0; i < bsize; i += 2)
  {
    c = fgetc(ifp);
    if ((blen[i] = c & 15) > 12 || (blen[i + 1] = c >> 4) > 12)
    {
      fseek(ifp, save, SEEK_SET);
      for (i = 0; i < bsize; i += 8)
      {
        read_shorts(raw, 6);
        out[i] = raw[0] >> 12 << 8 | raw[2] >> 12 << 4 | raw[4] >> 12;
        out[i + 1] = raw[1] >> 12 << 8 | raw[3] >> 12 << 4 | raw[5] >> 12;
        for (j = 0; j < 6; j++)
          out[i + 2 + j] = raw[j] & 0xfff;
      }
      return 1;
    }
  }
  if ((bsize & 7) == 4)
  {
    bitbuf = fgetc(ifp) << 8;
    bitbuf += fgetc(ifp);
    bits = 16;
  }
  for (i = 0; i < bsize; i++)
  {
    len = blen[i];
    if (bits < len)
    {
      for (j = 0; j < 32; j += 8)
        bitbuf += (INT64)fgetc(ifp) << (bits + (j ^ 8));
      bits += 32;
    }
    diff = bitbuf & (0xffff >> (16 - len));
    bitbuf >>= len;
    bits -= len;
    if ((diff & (1 << (len - 1))) == 0)
      diff -= (1 << len) - 1;
    out[i] = diff;
  }
  return 0;
}