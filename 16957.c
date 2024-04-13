gdImageFill (gdImagePtr im, int x, int y, int color)
{
  int lastBorder;
  int old;
  int leftLimit, rightLimit;
  int i;
  
  if (x >= im->sx) {
  	x = im->sx - 1;
  }
  
  if (y >= im->sy) {
  	y = im->sy - 1;
  }
  
  old = gdImageGetPixel (im, x, y);
  if (color == gdTiled)
    {
      /* Tile fill -- got to watch out! */
      int p, tileColor;
      int srcx, srcy;
      if (!im->tile)
	{
	  return;
	}
      /* Refuse to flood-fill with a transparent pattern --
         I can't do it without allocating another image */
      if (gdImageGetTransparent (im->tile) != (-1))
	{
	  return;
	}
      srcx = x % gdImageSX (im->tile);
      srcy = y % gdImageSY (im->tile);
      p = gdImageGetPixel (im->tile, srcx, srcy);
      if (im->trueColor)
	{
	  tileColor = p;
	}
      else
	{
	  if (im->tile->trueColor)
	    {
	      tileColor = gdImageColorResolveAlpha (im,
						    gdTrueColorGetRed (p),
						    gdTrueColorGetGreen (p),
						    gdTrueColorGetBlue (p),
						    gdTrueColorGetAlpha (p));
	    }
	  else
	    {
	      tileColor = im->tileColorMap[p];
	    }
	}
      if (old == tileColor)
	{
	  /* Nothing to be done */
	  return;
	}
    }
  else
    {
      if (old == color)
	{
	  /* Nothing to be done */
	  return;
	}
    }
  /* Seek left */
  leftLimit = (-1);
  for (i = x; (i >= 0); i--)
    {
      if (gdImageGetPixel (im, i, y) != old)
	{
	  break;
	}
      gdImageSetPixel (im, i, y, color);
      leftLimit = i;
    }
  if (leftLimit == (-1))
    {
      return;
    }
  /* Seek right */
  rightLimit = x;
  for (i = (x + 1); (i < im->sx); i++)
    {
      if (gdImageGetPixel (im, i, y) != old)
	{
	  break;
	}
      gdImageSetPixel (im, i, y, color);
      rightLimit = i;
    }
  /* Look at lines above and below and start paints */
  /* Above */
  if (y > 0)
    {
      lastBorder = 1;
      for (i = leftLimit; (i <= rightLimit); i++)
	{
	  int c;
	  c = gdImageGetPixel (im, i, y - 1);
	  if (lastBorder)
	    {
	      if (c == old)
		{
		  gdImageFill (im, i, y - 1, color);
		  lastBorder = 0;
		}
	    }
	  else if (c != old)
	    {
	      lastBorder = 1;
	    }
	}
    }
  /* Below */
  if (y < ((im->sy) - 1))
    {
      lastBorder = 1;
      for (i = leftLimit; (i <= rightLimit); i++)
	{
	  int c;
	  c = gdImageGetPixel (im, i, y + 1);
	  if (lastBorder)
	    {
	      if (c == old)
		{
		  gdImageFill (im, i, y + 1, color);
		  lastBorder = 0;
		}
	    }
	  else if (c != old)
	    {
	      lastBorder = 1;
	    }
	}
    }
}