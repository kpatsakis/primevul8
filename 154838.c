int32 GetCodedUIntSize(uint64 value) {
  if (value < 0x000000000000007FULL)
    return 1;
  else if (value < 0x0000000000003FFFULL)
    return 2;
  else if (value < 0x00000000001FFFFFULL)
    return 3;
  else if (value < 0x000000000FFFFFFFULL)
    return 4;
  else if (value < 0x00000007FFFFFFFFULL)
    return 5;
  else if (value < 0x000003FFFFFFFFFFULL)
    return 6;
  else if (value < 0x0001FFFFFFFFFFFFULL)
    return 7;
  return 8;
}