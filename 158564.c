bits_set(unsigned int *selector, size_t selsize, int *bits)
{
for(; *bits != -1; ++bits)
  BIT_SET(selector, selsize, *bits);
}