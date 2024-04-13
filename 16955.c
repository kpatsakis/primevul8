gdImageCreate (int sx, int sy)
{
  int i;
  gdImagePtr im;
  im = (gdImage *) gdMalloc (sizeof (gdImage));
  memset (im, 0, sizeof (gdImage));
  /* Row-major ever since gd 1.3 */
  im->pixels = (unsigned char **) gdMalloc (sizeof (unsigned char *) * sy);
  im->polyInts = 0;
  im->polyAllocated = 0;
  im->brush = 0;
  im->tile = 0;
  im->style = 0;
  for (i = 0; (i < sy); i++)
    {
      /* Row-major ever since gd 1.3 */
      im->pixels[i] = (unsigned char *) gdCalloc (
						sx, sizeof (unsigned char));
    }
  im->sx = sx;
  im->sy = sy;
  im->colorsTotal = 0;
  im->transparent = (-1);
  im->interlace = 0;
  im->thick = 1;
  for (i = 0; (i < gdMaxColors); i++)
    {
      im->open[i] = 1;
      im->red[i] = 0;
      im->green[i] = 0;
      im->blue[i] = 0;
    };
  im->trueColor = 0;
  im->tpixels = 0;
  return im;
}