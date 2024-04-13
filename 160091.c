void LibRaw::android_loose_load_raw()
{
  uchar *data, *dp;
  int bwide, row, col, c;
  UINT64 bitbuf = 0;

  bwide = (raw_width + 5) / 6 << 3;
  data = (uchar *)malloc(bwide);
  merror(data, "android_loose_load_raw()");
  for (row = 0; row < raw_height; row++)
  {
    if (fread(data, 1, bwide, ifp) < bwide)
      derror();
    for (dp = data, col = 0; col < raw_width; dp += 8, col += 6)
    {
      FORC(8) bitbuf = (bitbuf << 8) | dp[c ^ 7];
      FORC(6) RAW(row, col + c) = (bitbuf >> c * 10) & 0x3ff;
    }
  }
  free(data);
}