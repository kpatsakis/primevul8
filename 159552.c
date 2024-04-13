MagickExport Image *StatisticImageChannel(const Image *image,
  const ChannelType channel,const StatisticType type,const size_t width,
  const size_t height,ExceptionInfo *exception)
{
#define StatisticImageTag  "Statistic/Image"

  CacheView
    *image_view,
    *statistic_view;

  Image
    *statistic_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelList
    **magick_restrict pixel_list;

  size_t
    neighbor_height,
    neighbor_width;

  ssize_t
    y;

  /*
    Initialize statistics image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  statistic_image=CloneImage(image,0,0,MagickTrue,exception);
  if (statistic_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(statistic_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&statistic_image->exception);
      statistic_image=DestroyImage(statistic_image);
      return((Image *) NULL);
    }
  neighbor_width=width == 0 ? GetOptimalKernelWidth2D((double) width,0.5) :
    width;
  neighbor_height=height == 0 ? GetOptimalKernelWidth2D((double) height,0.5) :
    height;
  pixel_list=AcquirePixelListThreadSet(neighbor_width,neighbor_height);
  if (pixel_list == (PixelList **) NULL)
    {
      statistic_image=DestroyImage(statistic_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Make each pixel the min / max / median / mode / etc. of the neighborhood.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  statistic_view=AcquireAuthenticCacheView(statistic_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,statistic_image,statistic_image->rows,1)
#endif
  for (y=0; y < (ssize_t) statistic_image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p;

    register IndexPacket
      *magick_restrict statistic_indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-((ssize_t) neighbor_width/2L),y-
      (ssize_t) (neighbor_height/2L),image->columns+neighbor_width,
      neighbor_height,exception);
    q=QueueCacheViewAuthenticPixels(statistic_view,0,y,statistic_image->columns,      1,exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewVirtualIndexQueue(image_view);
    statistic_indexes=GetCacheViewAuthenticIndexQueue(statistic_view);
    for (x=0; x < (ssize_t) statistic_image->columns; x++)
    {
      MagickPixelPacket
        pixel;

      register const IndexPacket
        *magick_restrict s;

      register const PixelPacket
        *magick_restrict r;

      register ssize_t
        u,
        v;

      r=p;
      s=indexes+x;
      ResetPixelList(pixel_list[id]);
      for (v=0; v < (ssize_t) neighbor_height; v++)
      {
        for (u=0; u < (ssize_t) neighbor_width; u++)
          InsertPixelList(image,r+u,s+u,pixel_list[id]);
        r+=image->columns+neighbor_width;
        s+=image->columns+neighbor_width;
      }
      GetMagickPixelPacket(image,&pixel);
      SetMagickPixelPacket(image,p+neighbor_width*neighbor_height/2,indexes+x+
        neighbor_width*neighbor_height/2,&pixel);
      switch (type)
      {
        case GradientStatistic:
        {
          MagickPixelPacket
            maximum,
            minimum;

          GetMinimumPixelList(pixel_list[id],&pixel);
          minimum=pixel;
          GetMaximumPixelList(pixel_list[id],&pixel);
          maximum=pixel;
          pixel.red=MagickAbsoluteValue(maximum.red-minimum.red);
          pixel.green=MagickAbsoluteValue(maximum.green-minimum.green);
          pixel.blue=MagickAbsoluteValue(maximum.blue-minimum.blue);
          pixel.opacity=MagickAbsoluteValue(maximum.opacity-minimum.opacity);
          if (image->colorspace == CMYKColorspace)
            pixel.index=MagickAbsoluteValue(maximum.index-minimum.index);
          break;
        }
        case MaximumStatistic:
        {
          GetMaximumPixelList(pixel_list[id],&pixel);
          break;
        }
        case MeanStatistic:
        {
          GetMeanPixelList(pixel_list[id],&pixel);
          break;
        }
        case MedianStatistic:
        default:
        {
          GetMedianPixelList(pixel_list[id],&pixel);
          break;
        }
        case MinimumStatistic:
        {
          GetMinimumPixelList(pixel_list[id],&pixel);
          break;
        }
        case ModeStatistic:
        {
          GetModePixelList(pixel_list[id],&pixel);
          break;
        }
        case NonpeakStatistic:
        {
          GetNonpeakPixelList(pixel_list[id],&pixel);
          break;
        }
        case RootMeanSquareStatistic:
        {
          GetRootMeanSquarePixelList(pixel_list[id],&pixel);
          break;
        }
        case StandardDeviationStatistic:
        {
          GetStandardDeviationPixelList(pixel_list[id],&pixel);
          break;
        }
      }
      if ((channel & RedChannel) != 0)
        SetPixelRed(q,ClampToQuantum(pixel.red));
      if ((channel & GreenChannel) != 0)
        SetPixelGreen(q,ClampToQuantum(pixel.green));
      if ((channel & BlueChannel) != 0)
        SetPixelBlue(q,ClampToQuantum(pixel.blue));
      if ((channel & OpacityChannel) != 0)
        SetPixelOpacity(q,ClampToQuantum(pixel.opacity));
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace))
        SetPixelIndex(statistic_indexes+x,ClampToQuantum(pixel.index));
      p++;
      q++;
    }
    if (SyncCacheViewAuthenticPixels(statistic_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,StatisticImageTag,progress++,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  statistic_view=DestroyCacheView(statistic_view);
  image_view=DestroyCacheView(image_view);
  pixel_list=DestroyPixelListThreadSet(pixel_list);
  if (status == MagickFalse)
    statistic_image=DestroyImage(statistic_image);
  return(statistic_image);
}