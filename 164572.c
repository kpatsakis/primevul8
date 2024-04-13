static MagickBooleanType ForwardQuadrantSwap(const size_t width,
  const size_t height,double *source_pixels,double *forward_pixels)
{
  MagickBooleanType
    status;

  register ssize_t
    x;

  ssize_t
    center,
    y;

  /*
    Swap quadrants.
  */
  center=(ssize_t) (width/2L)+1L;
  status=RollFourier((size_t) center,height,0L,(ssize_t) height/2L,
    source_pixels);
  if (status == MagickFalse)
    return(MagickFalse);
  for (y=0L; y < (ssize_t) height; y++)
    for (x=0L; x < (ssize_t) (width/2L); x++)
      forward_pixels[y*width+x+width/2L]=source_pixels[y*center+x];
  for (y=1; y < (ssize_t) height; y++)
    for (x=0L; x < (ssize_t) (width/2L); x++)
      forward_pixels[(height-y)*width+width/2L-x-1L]=
        source_pixels[y*center+x+1L];
  for (x=0L; x < (ssize_t) (width/2L); x++)
    forward_pixels[width/2L-x-1L]=source_pixels[x+1L];
  return(MagickTrue);
}