int32 GetUIntSize(uint64 value) {
  if (value < 0x0000000000000100ULL)
    return 1;
  else if (value < 0x0000000000010000ULL)
    return 2;
  else if (value < 0x0000000001000000ULL)
    return 3;
  else if (value < 0x0000000100000000ULL)
    return 4;
  else if (value < 0x0000010000000000ULL)
    return 5;
  else if (value < 0x0001000000000000ULL)
    return 6;
  else if (value < 0x0100000000000000ULL)
    return 7;
  return 8;
}