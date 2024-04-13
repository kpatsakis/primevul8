MagickExport MagickBooleanType EvaluateImageChannel(Image *image,
  const ChannelType channel,const MagickEvaluateOperator op,const double value,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  RandomInfo
    **magick_restrict random_info;

  ssize_t
    y;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
  unsigned long
    key;
#endif

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
  status=MagickTrue;
  progress=0;
  random_info=AcquireRandomInfoThreadSet();
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  key=GetRandomSecretKey(random_info[0]);
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,key == ~0UL)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    register IndexPacket
      *magick_restrict indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

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
      MagickRealType
        result;

      if ((channel & RedChannel) != 0)
        {
          result=ApplyEvaluateOperator(random_info[id],GetPixelRed(q),op,value);
          if (op == MeanEvaluateOperator)
            result/=2.0;
          SetPixelRed(q,ClampToQuantum(result));
        }
      if ((channel & GreenChannel) != 0)
        {
          result=ApplyEvaluateOperator(random_info[id],GetPixelGreen(q),op,
            value);
          if (op == MeanEvaluateOperator)
            result/=2.0;
          SetPixelGreen(q,ClampToQuantum(result));
        }
      if ((channel & BlueChannel) != 0)
        {
          result=ApplyEvaluateOperator(random_info[id],GetPixelBlue(q),op,
            value);
          if (op == MeanEvaluateOperator)
            result/=2.0;
          SetPixelBlue(q,ClampToQuantum(result));
        }
      if ((channel & OpacityChannel) != 0)
        {
          if (image->matte == MagickFalse)
            {
              result=ApplyEvaluateOperator(random_info[id],GetPixelOpacity(q),
                op,value);
              if (op == MeanEvaluateOperator)
                result/=2.0;
              SetPixelOpacity(q,ClampToQuantum(result));
            }
          else
            {
              result=ApplyEvaluateOperator(random_info[id],GetPixelAlpha(q),
                op,value);
              if (op == MeanEvaluateOperator)
                result/=2.0;
              SetPixelAlpha(q,ClampToQuantum(result));
            }
        }
      if (((channel & IndexChannel) != 0) && (indexes != (IndexPacket *) NULL))
        {
          result=ApplyEvaluateOperator(random_info[id],GetPixelIndex(indexes+x),
            op,value);
          if (op == MeanEvaluateOperator)
            result/=2.0;
          SetPixelIndex(indexes+x,ClampToQuantum(result));
        }
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,EvaluateImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  random_info=DestroyRandomInfoThreadSet(random_info);
  return(status);
}