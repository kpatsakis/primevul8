static MagickBooleanType RollFourier(const size_t width,const size_t height,
  const ssize_t x_offset,const ssize_t y_offset,double *roll_pixels)
{
  double
    *source_pixels;

  MemoryInfo
    *source_info;

  register ssize_t
    i,
    x;

  ssize_t
    u,
    v,
    y;

  /*
    Move zero frequency (DC, average color) from (0,0) to (width/2,height/2).
  */
  source_info=AcquireVirtualMemory(width,height*sizeof(*source_pixels));
  if (source_info == (MemoryInfo *) NULL)
    return(MagickFalse);
  source_pixels=(double *) GetVirtualMemoryBlob(source_info);
  i=0L;
  for (y=0L; y < (ssize_t) height; y++)
  {
    if (y_offset < 0L)
      v=((y+y_offset) < 0L) ? y+y_offset+(ssize_t) height : y+y_offset;
    else
      v=((y+y_offset) > ((ssize_t) height-1L)) ? y+y_offset-(ssize_t) height :
        y+y_offset;
    for (x=0L; x < (ssize_t) width; x++)
    {
      if (x_offset < 0L)
        u=((x+x_offset) < 0L) ? x+x_offset+(ssize_t) width : x+x_offset;
      else
        u=((x+x_offset) > ((ssize_t) width-1L)) ? x+x_offset-(ssize_t) width :
          x+x_offset;
      source_pixels[v*width+u]=roll_pixels[i++];
    }
  }
  (void) memcpy(roll_pixels,source_pixels,height*width*
    sizeof(*source_pixels));
  source_info=RelinquishVirtualMemory(source_info);
  return(MagickTrue);
}