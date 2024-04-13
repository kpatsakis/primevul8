MagickExport MagickBooleanType GetImageChannelKurtosis(const Image *image,
  const ChannelType channel,double *kurtosis,double *skewness,
  ExceptionInfo *exception)
{
  double
    area,
    mean,
    standard_deviation,
    sum_squares,
    sum_cubes,
    sum_fourth_power;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  *kurtosis=0.0;
  *skewness=0.0;
  area=0.0;
  mean=0.0;
  standard_deviation=0.0;
  sum_squares=0.0;
  sum_cubes=0.0;
  sum_fourth_power=0.0;
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
      if ((channel & RedChannel) != 0)
        {
          mean+=GetPixelRed(p);
          sum_squares+=(double) GetPixelRed(p)*GetPixelRed(p);
          sum_cubes+=(double) GetPixelRed(p)*GetPixelRed(p)*GetPixelRed(p);
          sum_fourth_power+=(double) GetPixelRed(p)*GetPixelRed(p)*
            GetPixelRed(p)*GetPixelRed(p);
          area++;
        }
      if ((channel & GreenChannel) != 0)
        {
          mean+=GetPixelGreen(p);
          sum_squares+=(double) GetPixelGreen(p)*GetPixelGreen(p);
          sum_cubes+=(double) GetPixelGreen(p)*GetPixelGreen(p)*
            GetPixelGreen(p);
          sum_fourth_power+=(double) GetPixelGreen(p)*GetPixelGreen(p)*
            GetPixelGreen(p)*GetPixelGreen(p);
          area++;
        }
      if ((channel & BlueChannel) != 0)
        {
          mean+=GetPixelBlue(p);
          sum_squares+=(double) GetPixelBlue(p)*GetPixelBlue(p);
          sum_cubes+=(double) GetPixelBlue(p)*GetPixelBlue(p)*GetPixelBlue(p);
          sum_fourth_power+=(double) GetPixelBlue(p)*GetPixelBlue(p)*
            GetPixelBlue(p)*GetPixelBlue(p);
          area++;
        }
      if ((channel & OpacityChannel) != 0)
        {
          mean+=GetPixelAlpha(p);
          sum_squares+=(double) GetPixelOpacity(p)*GetPixelAlpha(p);
          sum_cubes+=(double) GetPixelOpacity(p)*GetPixelAlpha(p)*
            GetPixelAlpha(p);
          sum_fourth_power+=(double) GetPixelAlpha(p)*GetPixelAlpha(p)*
            GetPixelAlpha(p)*GetPixelAlpha(p);
          area++;
        }
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace))
        {
          double
            index;

          index=(double) GetPixelIndex(indexes+x);
          mean+=index;
          sum_squares+=index*index;
          sum_cubes+=index*index*index;
          sum_fourth_power+=index*index*index*index;
          area++;
        }
      p++;
    }
  }
  if (y < (ssize_t) image->rows)
    return(MagickFalse);
  if (area != 0.0)
    {
      mean/=area;
      sum_squares/=area;
      sum_cubes/=area;
      sum_fourth_power/=area;
    }
  standard_deviation=sqrt(sum_squares-(mean*mean));
  if (standard_deviation != 0.0)
    {
      *kurtosis=sum_fourth_power-4.0*mean*sum_cubes+6.0*mean*mean*sum_squares-
        3.0*mean*mean*mean*mean;
      *kurtosis/=standard_deviation*standard_deviation*standard_deviation*
        standard_deviation;
      *kurtosis-=3.0;
      *skewness=sum_cubes-3.0*mean*sum_squares+2.0*mean*mean*mean;
      *skewness/=standard_deviation*standard_deviation*standard_deviation;
    }
  return(y == (ssize_t) image->rows ? MagickTrue : MagickFalse);
}