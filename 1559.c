  inline int32 ByteSwapInt32ForBigEndian(int32_t x) {
    if (!port::kLittleEndian) {
      return BYTE_SWAP_32(x);
    } else {
      return x;
    }
  }