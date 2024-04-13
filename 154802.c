int32 WriteUIntSize(IMkvWriter* writer, uint64 value, int32 size) {
  if (!writer || size < 0 || size > 8)
    return -1;

  if (size > 0) {
    const uint64 bit = 1LL << (size * 7);

    if (value > (bit - 2))
      return -1;

    value |= bit;
  } else {
    size = 1;
    int64 bit;

    for (;;) {
      bit = 1LL << (size * 7);
      const uint64 max = bit - 2;

      if (value <= max)
        break;

      ++size;
    }

    if (size > 8)
      return false;

    value |= bit;
  }

  return SerializeInt(writer, value, size);
}