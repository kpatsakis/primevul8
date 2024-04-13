void LibRaw::tiff_set(struct tiff_hdr *th, ushort *ntag, ushort tag,
                      ushort type, int count, int val)
{
  struct libraw_tiff_tag *tt;
  int c;

  tt = (struct libraw_tiff_tag *)(ntag + 1) + (*ntag)++;
  tt->val.i = val;
  if (type == 1 && count <= 4)
    FORC(4) tt->val.c[c] = val >> (c << 3);
  else if (type == 2)
  {
    count = strnlen((char *)th + val, count - 1) + 1;
    if (count <= 4)
      FORC(4) tt->val.c[c] = ((char *)th)[val + c];
  }
  else if (type == 3 && count <= 2)
    FORC(2) tt->val.s[c] = val >> (c << 4);
  tt->count = count;
  tt->type = type;
  tt->tag = tag;
}