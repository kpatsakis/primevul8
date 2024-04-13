bits_clear(unsigned int *selector, size_t selsize, int *bits)
{
for(; *bits != -1; ++bits)
  BIT_CLEAR(selector, selsize, *bits);
}