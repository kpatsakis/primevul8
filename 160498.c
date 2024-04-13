void LibRaw::canon_600_load_raw()
{
  uchar data[1120], *dp;
  ushort *pix;
  int irow, row;

  for (irow = row = 0; irow < height; irow++)
  {
    checkCancel();
    if (fread(data, 1, 1120, ifp) < 1120)
      derror();
    pix = raw_image + row * raw_width;
    for (dp = data; dp < data + 1120; dp += 10, pix += 8)
    {
      pix[0] = (dp[0] << 2) + (dp[1] >> 6);
      pix[1] = (dp[2] << 2) + (dp[1] >> 4 & 3);
      pix[2] = (dp[3] << 2) + (dp[1] >> 2 & 3);
      pix[3] = (dp[4] << 2) + (dp[1] & 3);
      pix[4] = (dp[5] << 2) + (dp[9] & 3);
      pix[5] = (dp[6] << 2) + (dp[9] >> 2 & 3);
      pix[6] = (dp[7] << 2) + (dp[9] >> 4 & 3);
      pix[7] = (dp[8] << 2) + (dp[9] >> 6);
    }
    if ((row += 2) > height)
      row = 1;
  }
}