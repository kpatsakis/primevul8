static inline int bitDiff(int value1, int value2)
{
  int decBits = 0;
  if (value2 < value1)
    while (decBits <= 14 && (value2 << ++decBits) < value1)
      ;
  return decBits;
}