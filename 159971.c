inline unsigned int __DNG_HalfToFloat(ushort halfValue)
{
  int sign = (halfValue >> 15) & 0x00000001;
  int exponent = (halfValue >> 10) & 0x0000001f;
  int mantissa = halfValue & 0x000003ff;
  if (exponent == 0)
  {
    if (mantissa == 0)
    {
      return (unsigned int)(sign << 31);
    }
    else
    {
      while (!(mantissa & 0x00000400))
      {
        mantissa <<= 1;
        exponent -= 1;
      }
      exponent += 1;
      mantissa &= ~0x00000400;
    }
  }
  else if (exponent == 31)
  {
    if (mantissa == 0)
    {
      return (unsigned int)((sign << 31) | ((0x1eL + 127 - 15) << 23) |
                            (0x3ffL << 13));
    }
    else
    {
      return 0;
    }
  }
  exponent += (127 - 15);
  mantissa <<= 13;
  return (unsigned int)((sign << 31) | (exponent << 23) | mantissa);
}