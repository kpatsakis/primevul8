  inline int16 ByteSwapInt16ForBigEndian(int16_t x) {
    if (!port::kLittleEndian) {
      return BYTE_SWAP_16(x);
    } else {
      return x;
    }
  }