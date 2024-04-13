gdImageTileApply (gdImagePtr im, int x, int y)
{
  int srcx, srcy;
  int p;
  if (!im->tile)
    {
      return;
    }
  srcx = x % gdImageSX (im->tile);
  srcy = y % gdImageSY (im->tile);
  if (im->trueColor)
    {
      p = gdImageGetTrueColorPixel (im->tile, srcx, srcy);
      gdImageSetPixel (im, x, y, p);
    }
  else
    {
      p = gdImageGetPixel(im->tile, srcx, srcy);
      /* Allow for transparency */
      if (p != gdImageGetTransparent (im->tile))
	{
	  if (im->tile->trueColor)
	    {
	      /* Truecolor tile. Very slow
	         on a palette destination. */
	      gdImageSetPixel (im, x, y,
			       gdImageColorResolveAlpha (
							  im,
						      gdTrueColorGetRed (p),
						    gdTrueColorGetGreen (p),
						     gdTrueColorGetBlue (p),
						  gdTrueColorGetAlpha (p)));
	    }
	  else
	    {
	      gdImageSetPixel (im, x, y,
			       im->tileColorMap[p]);
	    }
	}
    }
}