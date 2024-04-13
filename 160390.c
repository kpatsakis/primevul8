void LibRaw::broadcom_load_raw()
{

  uchar *data, *dp;
  int rev, row, col, c;
  ushort raw_stride = (ushort)load_flags;
  rev = 3 * (order == 0x4949);
  data = (uchar *)malloc(raw_stride * 2);
  merror(data, "broadcom_load_raw()");

  for (row = 0; row < raw_height; row++)
  {
    if (fread(data + raw_stride, 1, raw_stride, ifp) < raw_stride)
      derror();
    FORC(raw_stride) data[c] = data[raw_stride + (c ^ rev)];
    for (dp = data, col = 0; col < raw_width; dp += 5, col += 4)
      FORC4 RAW(row, col + c) = (dp[c] << 2) | (dp[4] >> (c << 1) & 3);
  }
  free(data);
}