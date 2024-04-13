void LibRaw::canon_600_correct()
{
  int row, col, val;
  static const short mul[4][2] = {
      {1141, 1145}, {1128, 1109}, {1178, 1149}, {1128, 1109}};

  for (row = 0; row < height; row++)
  {
    checkCancel();
    for (col = 0; col < width; col++)
    {
      if ((val = BAYER(row, col) - black) < 0)
        val = 0;
      val = val * mul[row & 3][col & 1] >> 9;
      BAYER(row, col) = val;
    }
  }
  canon_600_fixed_wb(1311);
  canon_600_auto_wb();
  canon_600_coeff();
  maximum = (0x3ff - black) * 1109 >> 9;
  black = 0;
}