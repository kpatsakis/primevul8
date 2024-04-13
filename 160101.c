void LibRaw::jpeg_thumb()
{
  char *thumb;

  thumb = (char *)malloc(thumb_length);
  merror(thumb, "jpeg_thumb()");
  fread(thumb, 1, thumb_length, ifp);
  jpeg_thumb_writer(ofp, thumb, thumb_length);
  free(thumb);
}