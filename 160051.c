  int FC(int row, int col)
  {
    return (imgdata.idata.filters >> (((row << 1 & 14) | (col & 1)) << 1) & 3);
  }