void write_tiff(int width, int height, unsigned short *bitmap, const char *fn)
{
  struct tiff_hdr th;

  FILE *ofp = fopen(fn, "wb");
  if (!ofp)
    return;
  tiff_head(width, height, &th);
  fwrite(&th, sizeof th, 1, ofp);
  fwrite(bitmap, 2, width * height, ofp);
  fclose(ofp);
}