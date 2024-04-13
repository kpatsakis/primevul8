void LibRaw::android_tight_load_raw()
{
  uchar *data, *dp;
  int bwide, row, col, c;

  bwide = -(-5 * raw_width >> 5) << 3;
  data = (uchar *)malloc(bwide);
  merror(data, "android_tight_load_raw()");
  for (row = 0; row < raw_height; row++)
  {
    if (fread(data, 1, bwide, ifp) < bwide)
      derror();
    for (dp = data, col = 0; col < raw_width; dp += 5, col += 4)
      FORC4 RAW(row, col + c) = (dp[c] << 2) | (dp[4] >> (c << 1) & 3);
  }
  free(data);
}