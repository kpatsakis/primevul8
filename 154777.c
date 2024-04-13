int32 GetIntSize(int64 value) {
  // Doubling the requested value ensures positive values with their high bit
  // set are written with 0-padding to avoid flipping the signedness.
  const uint64 v = (value < 0) ? value ^ -1LL : value;
  return GetUIntSize(2 * v);
}