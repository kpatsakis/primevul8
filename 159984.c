void write_ppm(unsigned width, unsigned height, unsigned short *bitmap,
               const char *fname)
{
  if (!bitmap)
    return;

  FILE *f = fopen(fname, "wb");
  if (!f)
    return;
  int bits = 16;
  fprintf(f, "P5\n%d %d\n%d\n", width, height, (1 << bits) - 1);
  unsigned char *data = (unsigned char *)bitmap;
  unsigned data_size = width * height * 2;
#define SWAP(a, b)                                                             \
  {                                                                            \
    a ^= b;                                                                    \
    a ^= (b ^= a);                                                             \
  }
  for (unsigned i = 0; i < data_size; i += 2)
    SWAP(data[i], data[i + 1]);
#undef SWAP
  fwrite(data, data_size, 1, f);
  fclose(f);
}