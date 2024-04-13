header_checksum (const hive_h *h)
{
  uint32_t *daddr = (uint32_t *) h->addr;
  size_t i;
  uint32_t sum = 0;

  for (i = 0; i < 0x1fc / 4; ++i) {
    sum ^= le32toh (*daddr);
    daddr++;
  }

  return sum;
}