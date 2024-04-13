bitset_on_num(BitSetRef bs)
{
  int i, n;

  n = 0;
  for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
    if (BITSET_AT(bs, i)) n++;
  }

  return n;
}