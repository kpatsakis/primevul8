MagickExport ChannelStatistics *GetImageChannelStatistics(const Image *image,
  ExceptionInfo *exception)
{
  ChannelStatistics
    *channel_statistics;

  double
    area,
    standard_deviation;

  MagickPixelPacket
    number_bins,
    *histogram;

  QuantumAny
    range;

  register ssize_t
    i;

  size_t
    channels,
    depth,
    length;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  length=CompositeChannels+1UL;
  channel_statistics=(ChannelStatistics *) AcquireQuantumMemory(length,
    sizeof(*channel_statistics));
  histogram=(MagickPixelPacket *) AcquireQuantumMemory(MaxMap+1U,
    sizeof(*histogram));
  if ((channel_statistics == (ChannelStatistics *) NULL) ||
      (histogram == (MagickPixelPacket *) NULL))
    {
      if (histogram != (MagickPixelPacket *) NULL)
        histogram=(MagickPixelPacket *) RelinquishMagickMemory(histogram);
      if (channel_statistics != (ChannelStatistics *) NULL)
        channel_statistics=(ChannelStatistics *) RelinquishMagickMemory(
          channel_statistics);
      return(channel_statistics);
    }
  (void) memset(channel_statistics,0,length*
    sizeof(*channel_statistics));
  for (i=0; i <= (ssize_t) CompositeChannels; i++)
  {
    channel_statistics[i].depth=1;
    channel_statistics[i].maxima=(-MagickMaximumValue);
    channel_statistics[i].minima=MagickMaximumValue;
  }
  (void) memset(histogram,0,(MaxMap+1U)*sizeof(*histogram));
  (void) memset(&number_bins,0,sizeof(number_bins));
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    /*
      Compute pixel statistics.
    */
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; )
    {
      if (channel_statistics[RedChannel].depth != MAGICKCORE_QUANTUM_DEPTH)
        {
          depth=channel_statistics[RedChannel].depth;
          range=GetQuantumRange(depth);
          if (IsPixelAtDepth(GetPixelRed(p),range) == MagickFalse)
            {
              channel_statistics[RedChannel].depth++;
              continue;
            }
        }
      if (channel_statistics[GreenChannel].depth != MAGICKCORE_QUANTUM_DEPTH)
        {
          depth=channel_statistics[GreenChannel].depth;
          range=GetQuantumRange(depth);
          if (IsPixelAtDepth(GetPixelGreen(p),range) == MagickFalse)
            {
              channel_statistics[GreenChannel].depth++;
              continue;
            }
        }
      if (channel_statistics[BlueChannel].depth != MAGICKCORE_QUANTUM_DEPTH)
        {
          depth=channel_statistics[BlueChannel].depth;
          range=GetQuantumRange(depth);
          if (IsPixelAtDepth(GetPixelBlue(p),range) == MagickFalse)
            {
              channel_statistics[BlueChannel].depth++;
              continue;
            }
        }
      if (image->matte != MagickFalse)
        {
          if (channel_statistics[OpacityChannel].depth != MAGICKCORE_QUANTUM_DEPTH)
            {
              depth=channel_statistics[OpacityChannel].depth;
              range=GetQuantumRange(depth);
              if (IsPixelAtDepth(GetPixelAlpha(p),range) == MagickFalse)
                {
                  channel_statistics[OpacityChannel].depth++;
                  continue;
                }
            }
          }
      if (image->colorspace == CMYKColorspace)
        {
          if (channel_statistics[BlackChannel].depth != MAGICKCORE_QUANTUM_DEPTH)
            {
              depth=channel_statistics[BlackChannel].depth;
              range=GetQuantumRange(depth);
              if (IsPixelAtDepth(GetPixelIndex(indexes+x),range) == MagickFalse)
                {
                  channel_statistics[BlackChannel].depth++;
                  continue;
                }
            }
        }
      if ((double) GetPixelRed(p) < channel_statistics[RedChannel].minima)
        channel_statistics[RedChannel].minima=(double) GetPixelRed(p);
      if ((double) GetPixelRed(p) > channel_statistics[RedChannel].maxima)
        channel_statistics[RedChannel].maxima=(double) GetPixelRed(p);
      channel_statistics[RedChannel].sum+=GetPixelRed(p);
      channel_statistics[RedChannel].sum_squared+=(double) GetPixelRed(p)*
        GetPixelRed(p);
      channel_statistics[RedChannel].sum_cubed+=(double)
        GetPixelRed(p)*GetPixelRed(p)*GetPixelRed(p);
      channel_statistics[RedChannel].sum_fourth_power+=(double)
        GetPixelRed(p)*GetPixelRed(p)*GetPixelRed(p)*GetPixelRed(p);
      if ((double) GetPixelGreen(p) < channel_statistics[GreenChannel].minima)
        channel_statistics[GreenChannel].minima=(double) GetPixelGreen(p);
      if ((double) GetPixelGreen(p) > channel_statistics[GreenChannel].maxima)
        channel_statistics[GreenChannel].maxima=(double) GetPixelGreen(p);
      channel_statistics[GreenChannel].sum+=GetPixelGreen(p);
      channel_statistics[GreenChannel].sum_squared+=(double) GetPixelGreen(p)*
        GetPixelGreen(p);
      channel_statistics[GreenChannel].sum_cubed+=(double) GetPixelGreen(p)*
        GetPixelGreen(p)*GetPixelGreen(p);
      channel_statistics[GreenChannel].sum_fourth_power+=(double)
        GetPixelGreen(p)*GetPixelGreen(p)*GetPixelGreen(p)*GetPixelGreen(p);
      if ((double) GetPixelBlue(p) < channel_statistics[BlueChannel].minima)
        channel_statistics[BlueChannel].minima=(double) GetPixelBlue(p);
      if ((double) GetPixelBlue(p) > channel_statistics[BlueChannel].maxima)
        channel_statistics[BlueChannel].maxima=(double) GetPixelBlue(p);
      channel_statistics[BlueChannel].sum+=GetPixelBlue(p);
      channel_statistics[BlueChannel].sum_squared+=(double) GetPixelBlue(p)*
        GetPixelBlue(p);
      channel_statistics[BlueChannel].sum_cubed+=(double) GetPixelBlue(p)*
        GetPixelBlue(p)*GetPixelBlue(p);
      channel_statistics[BlueChannel].sum_fourth_power+=(double)
        GetPixelBlue(p)*GetPixelBlue(p)*GetPixelBlue(p)*GetPixelBlue(p);
      histogram[ScaleQuantumToMap(GetPixelRed(p))].red++;
      histogram[ScaleQuantumToMap(GetPixelGreen(p))].green++;
      histogram[ScaleQuantumToMap(GetPixelBlue(p))].blue++;
      if (image->matte != MagickFalse)
        {
          if ((double) GetPixelAlpha(p) < channel_statistics[OpacityChannel].minima)
            channel_statistics[OpacityChannel].minima=(double) GetPixelAlpha(p);
          if ((double) GetPixelAlpha(p) > channel_statistics[OpacityChannel].maxima)
            channel_statistics[OpacityChannel].maxima=(double) GetPixelAlpha(p);
          channel_statistics[OpacityChannel].sum+=GetPixelAlpha(p);
          channel_statistics[OpacityChannel].sum_squared+=(double)
            GetPixelAlpha(p)*GetPixelAlpha(p);
          channel_statistics[OpacityChannel].sum_cubed+=(double)
            GetPixelAlpha(p)*GetPixelAlpha(p)*GetPixelAlpha(p);
          channel_statistics[OpacityChannel].sum_fourth_power+=(double)
            GetPixelAlpha(p)*GetPixelAlpha(p)*GetPixelAlpha(p)*GetPixelAlpha(p);
          histogram[ScaleQuantumToMap(GetPixelAlpha(p))].opacity++;
        }
      if (image->colorspace == CMYKColorspace)
        {
          if ((double) GetPixelIndex(indexes+x) < channel_statistics[BlackChannel].minima)
            channel_statistics[BlackChannel].minima=(double)
              GetPixelIndex(indexes+x);
          if ((double) GetPixelIndex(indexes+x) > channel_statistics[BlackChannel].maxima)
            channel_statistics[BlackChannel].maxima=(double)
              GetPixelIndex(indexes+x);
          channel_statistics[BlackChannel].sum+=GetPixelIndex(indexes+x);
          channel_statistics[BlackChannel].sum_squared+=(double)
            GetPixelIndex(indexes+x)*GetPixelIndex(indexes+x);
          channel_statistics[BlackChannel].sum_cubed+=(double)
            GetPixelIndex(indexes+x)*GetPixelIndex(indexes+x)*
            GetPixelIndex(indexes+x);
          channel_statistics[BlackChannel].sum_fourth_power+=(double)
            GetPixelIndex(indexes+x)*GetPixelIndex(indexes+x)*
            GetPixelIndex(indexes+x)*GetPixelIndex(indexes+x);
          histogram[ScaleQuantumToMap(GetPixelIndex(indexes+x))].index++;
        }
      x++;
      p++;
    }
  }
  for (i=0; i < (ssize_t) CompositeChannels; i++)
  {
    double
      area,
      mean,
      standard_deviation;

    /*
      Normalize pixel statistics.
    */
    area=PerceptibleReciprocal((double) image->columns*image->rows);
    mean=channel_statistics[i].sum*area;
    channel_statistics[i].sum=mean;
    channel_statistics[i].sum_squared*=area;
    channel_statistics[i].sum_cubed*=area;
    channel_statistics[i].sum_fourth_power*=area;
    channel_statistics[i].mean=mean;
    channel_statistics[i].variance=channel_statistics[i].sum_squared;
    standard_deviation=sqrt(channel_statistics[i].variance-(mean*mean));
    area=PerceptibleReciprocal((double) image->columns*image->rows-1.0)*
      ((double) image->columns*image->rows);
    standard_deviation=sqrt(area*standard_deviation*standard_deviation);
    channel_statistics[i].standard_deviation=standard_deviation;
  }
  for (i=0; i < (ssize_t) (MaxMap+1U); i++)
  {
    if (histogram[i].red > 0.0)
      number_bins.red++;
    if (histogram[i].green > 0.0)
      number_bins.green++;
    if (histogram[i].blue > 0.0)
      number_bins.blue++;
    if ((image->matte != MagickFalse) && (histogram[i].opacity > 0.0))
      number_bins.opacity++;
    if ((image->colorspace == CMYKColorspace) && (histogram[i].index > 0.0))
      number_bins.index++;
  }
  area=PerceptibleReciprocal((double) image->columns*image->rows);
  for (i=0; i < (ssize_t) (MaxMap+1U); i++)
  {
    /*
      Compute pixel entropy.
    */
    histogram[i].red*=area;
    channel_statistics[RedChannel].entropy+=-histogram[i].red*
      MagickLog10(histogram[i].red)*
      PerceptibleReciprocal(MagickLog10((double) number_bins.red));
    histogram[i].green*=area;
    channel_statistics[GreenChannel].entropy+=-histogram[i].green*
      MagickLog10(histogram[i].green)*
      PerceptibleReciprocal(MagickLog10((double) number_bins.green));
    histogram[i].blue*=area;
    channel_statistics[BlueChannel].entropy+=-histogram[i].blue*
      MagickLog10(histogram[i].blue)*
      PerceptibleReciprocal(MagickLog10((double) number_bins.blue));
    if (image->matte != MagickFalse)
      {
        histogram[i].opacity*=area;
        channel_statistics[OpacityChannel].entropy+=-histogram[i].opacity*
          MagickLog10(histogram[i].opacity)*
          PerceptibleReciprocal(MagickLog10((double) number_bins.opacity));
      }
    if (image->colorspace == CMYKColorspace)
      {
        histogram[i].index*=area;
        channel_statistics[IndexChannel].entropy+=-histogram[i].index*
          MagickLog10(histogram[i].index)*
          PerceptibleReciprocal(MagickLog10((double) number_bins.index));
      }
  }
  /*
    Compute overall statistics.
  */
  for (i=0; i < (ssize_t) CompositeChannels; i++)
  {
    channel_statistics[CompositeChannels].depth=(size_t) EvaluateMax((double)
      channel_statistics[CompositeChannels].depth,(double)
      channel_statistics[i].depth);
    channel_statistics[CompositeChannels].minima=MagickMin(
      channel_statistics[CompositeChannels].minima,
      channel_statistics[i].minima);
    channel_statistics[CompositeChannels].maxima=EvaluateMax(
      channel_statistics[CompositeChannels].maxima,
      channel_statistics[i].maxima);
    channel_statistics[CompositeChannels].sum+=channel_statistics[i].sum;
    channel_statistics[CompositeChannels].sum_squared+=
      channel_statistics[i].sum_squared;
    channel_statistics[CompositeChannels].sum_cubed+=
      channel_statistics[i].sum_cubed;
    channel_statistics[CompositeChannels].sum_fourth_power+=
      channel_statistics[i].sum_fourth_power;
    channel_statistics[CompositeChannels].mean+=channel_statistics[i].mean;
    channel_statistics[CompositeChannels].variance+=
      channel_statistics[i].variance-channel_statistics[i].mean*
      channel_statistics[i].mean;
    standard_deviation=sqrt(channel_statistics[i].variance-
      (channel_statistics[i].mean*channel_statistics[i].mean));
    area=PerceptibleReciprocal((double) image->columns*image->rows-1.0)*
      ((double) image->columns*image->rows);
    standard_deviation=sqrt(area*standard_deviation*standard_deviation);
    channel_statistics[CompositeChannels].standard_deviation=standard_deviation;
    channel_statistics[CompositeChannels].entropy+=
      channel_statistics[i].entropy;
  }
  channels=3;
  if (image->matte != MagickFalse)
    channels++;
  if (image->colorspace == CMYKColorspace)
    channels++;
  channel_statistics[CompositeChannels].sum/=channels;
  channel_statistics[CompositeChannels].sum_squared/=channels;
  channel_statistics[CompositeChannels].sum_cubed/=channels;
  channel_statistics[CompositeChannels].sum_fourth_power/=channels;
  channel_statistics[CompositeChannels].mean/=channels;
  channel_statistics[CompositeChannels].kurtosis/=channels;
  channel_statistics[CompositeChannels].skewness/=channels;
  channel_statistics[CompositeChannels].entropy/=channels;
  i=CompositeChannels;
  area=PerceptibleReciprocal((double) channels*image->columns*image->rows);
  channel_statistics[i].variance=channel_statistics[i].sum_squared;
  channel_statistics[i].mean=channel_statistics[i].sum;
  standard_deviation=sqrt(channel_statistics[i].variance-
    (channel_statistics[i].mean*channel_statistics[i].mean));
  standard_deviation=sqrt(PerceptibleReciprocal((double) channels*
    image->columns*image->rows-1.0)*channels*image->columns*image->rows*
    standard_deviation*standard_deviation);
  channel_statistics[i].standard_deviation=standard_deviation;
  for (i=0; i <= (ssize_t) CompositeChannels; i++)
  {
    /*
      Compute kurtosis & skewness statistics.
    */
    standard_deviation=PerceptibleReciprocal(
      channel_statistics[i].standard_deviation);
    channel_statistics[i].skewness=(channel_statistics[i].sum_cubed-3.0*
      channel_statistics[i].mean*channel_statistics[i].sum_squared+2.0*
      channel_statistics[i].mean*channel_statistics[i].mean*
      channel_statistics[i].mean)*(standard_deviation*standard_deviation*
      standard_deviation);
    channel_statistics[i].kurtosis=(channel_statistics[i].sum_fourth_power-4.0*
      channel_statistics[i].mean*channel_statistics[i].sum_cubed+6.0*
      channel_statistics[i].mean*channel_statistics[i].mean*
      channel_statistics[i].sum_squared-3.0*channel_statistics[i].mean*
      channel_statistics[i].mean*1.0*channel_statistics[i].mean*
      channel_statistics[i].mean)*(standard_deviation*standard_deviation*
      standard_deviation*standard_deviation)-3.0;
  }
  channel_statistics[CompositeChannels].mean=0.0;
  channel_statistics[CompositeChannels].standard_deviation=0.0;
  for (i=0; i < (ssize_t) CompositeChannels; i++)
  {
    channel_statistics[CompositeChannels].mean+=
      channel_statistics[i].mean;
    channel_statistics[CompositeChannels].standard_deviation+=
      channel_statistics[i].standard_deviation;
  }
  channel_statistics[CompositeChannels].mean/=(double) channels;
  channel_statistics[CompositeChannels].standard_deviation/=(double) channels;
  histogram=(MagickPixelPacket *) RelinquishMagickMemory(histogram);
  if (y < (ssize_t) image->rows)
    channel_statistics=(ChannelStatistics *) RelinquishMagickMemory(
      channel_statistics);
  return(channel_statistics);
}