MagickExport MagickBooleanType FunctionImageChannel(Image *image,
  const ChannelType channel,const MagickFunction function,
  const size_t number_parameters,const double *parameters,
  ExceptionInfo *exception)
{
#define FunctionImageTag  "Function/Image "

  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(MagickFalse);
    }
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  status=AccelerateFunctionImage(image,channel,function,number_parameters,
    parameters,exception);
  if (status != MagickFalse)
    return(status);
#endif
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register IndexPacket
      *magick_restrict indexes;

    register ssize_t
      x;

    register PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((channel & RedChannel) != 0)
        SetPixelRed(q,ApplyFunction(GetPixelRed(q),function,
          number_parameters,parameters,exception));
      if ((channel & GreenChannel) != 0)
        SetPixelGreen(q,ApplyFunction(GetPixelGreen(q),function,
          number_parameters,parameters,exception));
      if ((channel & BlueChannel) != 0)
        SetPixelBlue(q,ApplyFunction(GetPixelBlue(q),function,
          number_parameters,parameters,exception));
      if ((channel & OpacityChannel) != 0)
        {
          if (image->matte == MagickFalse)
            SetPixelOpacity(q,ApplyFunction(GetPixelOpacity(q),function,
              number_parameters,parameters,exception));
          else
            SetPixelAlpha(q,ApplyFunction((Quantum) GetPixelAlpha(q),function,
              number_parameters,parameters,exception));
        }
      if (((channel & IndexChannel) != 0) && (indexes != (IndexPacket *) NULL))
        SetPixelIndex(indexes+x,ApplyFunction(GetPixelIndex(indexes+x),function,
          number_parameters,parameters,exception));
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,FunctionImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}