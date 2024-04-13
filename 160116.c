void LibRaw::get_mem_image_format(int *width, int *height, int *colors,
                                  int *bps) const

{
  if (S.flip & 4)
  {
    *width = S.height;
    *height = S.width;
  }
  else
  {
    *width = S.width;
    *height = S.height;
  }
  *colors = P1.colors;
  *bps = O.output_bps;
}