void tiff_set(ushort *ntag, ushort tag, ushort type, int count, int val)
{
  struct libraw_tiff_tag *tt;
  int c;

  tt = (struct libraw_tiff_tag *)(ntag + 1) + (*ntag)++;
  tt->tag = tag;
  tt->type = type;
  tt->count = count;
  if (type < 3 && count <= 4)
    for (c = 0; c < 4; c++)
      tt->val.c[c] = val >> (c << 3);
  else if (type == 3 && count <= 2)
    for (c = 0; c < 2; c++)
      tt->val.s[c] = val >> (c << 4);
  else
    tt->val.i = val;
}