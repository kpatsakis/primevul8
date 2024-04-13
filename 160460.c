void LibRaw::ppm16_thumb()
{
  int i;
  char *thumb;
  thumb_length = thumb_width * thumb_height * 3;
  thumb = (char *)calloc(thumb_length, 2);
  merror(thumb, "ppm16_thumb()");
  read_shorts((ushort *)thumb, thumb_length);
  for (i = 0; i < thumb_length; i++)
    thumb[i] = ((ushort *)thumb)[i] >> 8;
  fprintf(ofp, "P6\n%d %d\n255\n", thumb_width, thumb_height);
  fwrite(thumb, 1, thumb_length, ofp);
  free(thumb);
}