static int GetCutColors(Image *image)
{
  ExceptionInfo
    *exception;

  Quantum
    intensity,
    scale_intensity;

  register PixelPacket
    *q;

  ssize_t
    x,
    y;

  exception=(&image->exception);
  intensity=0;
  scale_intensity=ScaleCharToQuantum(16);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (intensity < GetPixelRed(q))
        intensity=GetPixelRed(q);
      if (intensity >= scale_intensity)
        return(255);
      q++;
    }
  }
  if (intensity < ScaleCharToQuantum(2))
    return(2);
  if (intensity < ScaleCharToQuantum(16))
    return(16);
  return((int) intensity);
}