bitset_or(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) { dest[i] |= bs[i]; }
}