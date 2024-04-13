gdImageRectangle (gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
  int x1h = x1, x1v = x1, y1h = y1, y1v = y1, x2h = x2, x2v = x2, y2h = y2,
    y2v = y2;
  int thick = im->thick;
  if (thick > 1)
    {
      int half = thick / 2;
      int half1 = thick - half;

      if (y1 < y2)
	{
	  y1v = y1h - half;
	  y2v = y2h + half1 - 1;
	}
      else
	{
	  y1v = y1h + half1 - 1;
	  y2v = y2h - half;
	}
    }

  gdImageLine (im, x1h, y1h, x2h, y1h, color);
  gdImageLine (im, x1h, y2h, x2h, y2h, color);
  gdImageLine (im, x1v, y1v, x1v, y2v, color);
  gdImageLine (im, x2v, y1v, x2v, y2v, color);
}