void LibRaw::nikon_3700()
{
  int bits, i;
  uchar dp[24];
  static const struct
  {
    int bits;
    char t_make[12], t_model[15];
  } table[] = {{0x00, "Pentax", "Optio 33WR"},
               {0x03, "Nikon", "E3200"},
               {0x32, "Nikon", "E3700"},
               {0x33, "Olympus", "C740UZ"}};

  fseek(ifp, 3072, SEEK_SET);
  fread(dp, 1, 24, ifp);
  bits = (dp[8] & 3) << 4 | (dp[20] & 3);
  for (i = 0; i < sizeof table / sizeof *table; i++)
    if (bits == table[i].bits)
    {
      strcpy(make, table[i].t_make);
      strcpy(model, table[i].t_model);
    }
}