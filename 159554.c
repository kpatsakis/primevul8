MagickExport MagickBooleanType GetImageChannelRange(const Image *image,
  const ChannelType channel,double *minima,double *maxima,
  ExceptionInfo *exception)
{
  MagickPixelPacket
    pixel;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  *maxima=(-MagickMaximumValue);
  *minima=MagickMaximumValue;
  GetMagickPixelPacket(image,&pixel);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetMagickPixelPacket(image,p,indexes+x,&pixel);
      if ((channel & RedChannel) != 0)
        {
          if (pixel.red < *minima)
            *minima=(double) pixel.red;
          if (pixel.red > *maxima)
            *maxima=(double) pixel.red;
        }
      if ((channel & GreenChannel) != 0)
        {
          if (pixel.green < *minima)
            *minima=(double) pixel.green;
          if (pixel.green > *maxima)
            *maxima=(double) pixel.green;
        }
      if ((channel & BlueChannel) != 0)
        {
          if (pixel.blue < *minima)
            *minima=(double) pixel.blue;
          if (pixel.blue > *maxima)
            *maxima=(double) pixel.blue;
        }
      if (((channel & OpacityChannel) != 0) && (image->matte != MagickFalse))
        {
          if ((QuantumRange-pixel.opacity) < *minima)
            *minima=(double) (QuantumRange-pixel.opacity);
          if ((QuantumRange-pixel.opacity) > *maxima)
            *maxima=(double) (QuantumRange-pixel.opacity);
        }
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace))
        {
          if ((double) pixel.index < *minima)
            *minima=(double) pixel.index;
          if ((double) pixel.index > *maxima)
            *maxima=(double) pixel.index;
        }
      p++;
    }
  }
  return(y == (ssize_t) image->rows ? MagickTrue : MagickFalse);
}