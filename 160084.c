inline unsigned int __DNG_FP24ToFloat(const unsigned char *input)
{
  int sign = (input[0] >> 7) & 0x01;
  int exponent = (input[0]) & 0x7F;
  int mantissa = (((int)input[1]) << 8) | input[2];
  if (exponent == 0)
  {
    if (mantissa == 0)
    {
      return (unsigned int)(sign << 31);
    }
    else
    {
      while (!(mantissa & 0x00010000))
      {
        mantissa <<= 1;
        exponent -= 1;
      }
      exponent += 1;
      mantissa &= ~0x00010000;
    }
  }
  else if (exponent == 127)
  {
    if (mantissa == 0)
    {
      return (unsigned int)((sign << 31) | ((0x7eL + 128 - 64) << 23) |
                            (0xffffL << 7));
    }
    else
    {
      // Nan -- Just set to zero.
      return 0;
    }
  }
  exponent += (128 - 64);
  mantissa <<= 7;
  return (uint32_t)((sign << 31) | (exponent << 23) | mantissa);
}