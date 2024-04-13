void LibRaw::lin_interpolate_loop(int code[16][16][32], int size)
{
  int row;
  for (row = 1; row < height - 1; row++)
  {
    int col, *ip;
    ushort *pix;
    for (col = 1; col < width - 1; col++)
    {
      int i;
      int sum[4];
      pix = image[row * width + col];
      ip = code[row % size][col % size];
      memset(sum, 0, sizeof sum);
      for (i = *ip++; i--; ip += 3)
        sum[ip[2]] += pix[ip[0]] << ip[1];
      for (i = colors; --i; ip += 2)
        pix[ip[0]] = sum[ip[0]] * ip[1] >> 8;
    }
  }
}