MagickExport Image *EvaluateImages(const Image *images,
  const MagickEvaluateOperator op,ExceptionInfo *exception)
{
#define EvaluateImageTag  "Evaluate/Image"

  CacheView
    *evaluate_view;

  Image
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    **magick_restrict evaluate_pixels,
    zero;

  RandomInfo
    **magick_restrict random_info;

  size_t
    number_images;

  ssize_t
    y;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
  unsigned long
    key;
#endif

  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImageCanvas(images,exception);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&image->exception);
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  evaluate_pixels=AcquirePixelThreadSet(images);
  if (evaluate_pixels == (MagickPixelPacket **) NULL)
    {
      image=DestroyImage(image);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",images->filename);
      return((Image *) NULL);
    }
  /*
    Evaluate image pixels.
  */
  status=MagickTrue;
  progress=0;
  number_images=GetImageListLength(images);
  GetMagickPixelPacket(images,&zero);
  random_info=AcquireRandomInfoThreadSet();
  evaluate_view=AcquireAuthenticCacheView(image,exception);
  if (op == MedianEvaluateOperator)
    {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      key=GetRandomSecretKey(random_info[0]);
      #pragma omp parallel for schedule(static) shared(progress,status) \
        magick_number_threads(image,images,image->rows,key == ~0UL)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        CacheView
          *image_view;

        const Image
          *next;

        const int
          id = GetOpenMPThreadId();

        register IndexPacket
          *magick_restrict evaluate_indexes;

        register MagickPixelPacket
          *evaluate_pixel;

        register PixelPacket
          *magick_restrict q;

        register ssize_t
          x;

        if (status == MagickFalse)
          continue;
        q=QueueCacheViewAuthenticPixels(evaluate_view,0,y,image->columns,1,
          exception);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        evaluate_indexes=GetCacheViewAuthenticIndexQueue(evaluate_view);
        evaluate_pixel=evaluate_pixels[id];
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          register ssize_t
            i;

          for (i=0; i < (ssize_t) number_images; i++)
            evaluate_pixel[i]=zero;
          next=images;
          for (i=0; i < (ssize_t) number_images; i++)
          {
            register const IndexPacket
              *indexes;

            register const PixelPacket
              *p;

            image_view=AcquireVirtualCacheView(next,exception);
            p=GetCacheViewVirtualPixels(image_view,x,y,1,1,exception);
            if (p == (const PixelPacket *) NULL)
              {
                image_view=DestroyCacheView(image_view);
                break;
              }
            indexes=GetCacheViewVirtualIndexQueue(image_view);
            evaluate_pixel[i].red=ApplyEvaluateOperator(random_info[id],
              GetPixelRed(p),op,evaluate_pixel[i].red);
            evaluate_pixel[i].green=ApplyEvaluateOperator(random_info[id],
              GetPixelGreen(p),op,evaluate_pixel[i].green);
            evaluate_pixel[i].blue=ApplyEvaluateOperator(random_info[id],
              GetPixelBlue(p),op,evaluate_pixel[i].blue);
            evaluate_pixel[i].opacity=ApplyEvaluateOperator(random_info[id],
              GetPixelAlpha(p),op,evaluate_pixel[i].opacity);
            if (image->colorspace == CMYKColorspace)
              evaluate_pixel[i].index=ApplyEvaluateOperator(random_info[id],
                *indexes,op,evaluate_pixel[i].index);
            image_view=DestroyCacheView(image_view);
            next=GetNextImageInList(next);
          }
          qsort((void *) evaluate_pixel,number_images,sizeof(*evaluate_pixel),
            IntensityCompare);
          SetPixelRed(q,ClampToQuantum(evaluate_pixel[i/2].red));
          SetPixelGreen(q,ClampToQuantum(evaluate_pixel[i/2].green));
          SetPixelBlue(q,ClampToQuantum(evaluate_pixel[i/2].blue));
          SetPixelAlpha(q,ClampToQuantum(evaluate_pixel[i/2].opacity));
          if (image->colorspace == CMYKColorspace)
            SetPixelIndex(evaluate_indexes+i,ClampToQuantum(
              evaluate_pixel[i/2].index));
          q++;
        }
        if (SyncCacheViewAuthenticPixels(evaluate_view,exception) == MagickFalse)
          status=MagickFalse;
        if (images->progress_monitor != (MagickProgressMonitor) NULL)
          {
            MagickBooleanType
              proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
            #pragma omp atomic
#endif
            progress++;
            proceed=SetImageProgress(images,EvaluateImageTag,progress,
              image->rows);
            if (proceed == MagickFalse)
              status=MagickFalse;
          }
      }
    }
  else
    {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      key=GetRandomSecretKey(random_info[0]);
      #pragma omp parallel for schedule(static) shared(progress,status) \
        magick_number_threads(image,images,image->rows,key == ~0UL)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        CacheView
          *image_view;

        const Image
          *next;

        const int
          id = GetOpenMPThreadId();

        register IndexPacket
          *magick_restrict evaluate_indexes;

        register ssize_t
          i,
          x;

        register MagickPixelPacket
          *evaluate_pixel;

        register PixelPacket
          *magick_restrict q;

        if (status == MagickFalse)
          continue;
        q=QueueCacheViewAuthenticPixels(evaluate_view,0,y,image->columns,1,
          exception);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        evaluate_indexes=GetCacheViewAuthenticIndexQueue(evaluate_view);
        evaluate_pixel=evaluate_pixels[id];
        for (x=0; x < (ssize_t) image->columns; x++)
          evaluate_pixel[x]=zero;
        next=images;
        for (i=0; i < (ssize_t) number_images; i++)
        {
          register const IndexPacket
            *indexes;

          register const PixelPacket
            *p;

          image_view=AcquireVirtualCacheView(next,exception);
          p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,
            exception);
          if (p == (const PixelPacket *) NULL)
            {
              image_view=DestroyCacheView(image_view);
              break;
            }
          indexes=GetCacheViewVirtualIndexQueue(image_view);
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            evaluate_pixel[x].red=ApplyEvaluateOperator(random_info[id],
              GetPixelRed(p),i == 0 ? AddEvaluateOperator : op,
              evaluate_pixel[x].red);
            evaluate_pixel[x].green=ApplyEvaluateOperator(random_info[id],
              GetPixelGreen(p),i == 0 ? AddEvaluateOperator : op,
              evaluate_pixel[x].green);
            evaluate_pixel[x].blue=ApplyEvaluateOperator(random_info[id],
              GetPixelBlue(p),i == 0 ? AddEvaluateOperator : op,
              evaluate_pixel[x].blue);
            evaluate_pixel[x].opacity=ApplyEvaluateOperator(random_info[id],
              GetPixelAlpha(p),i == 0 ? AddEvaluateOperator : op,
              evaluate_pixel[x].opacity);
            if (image->colorspace == CMYKColorspace)
              evaluate_pixel[x].index=ApplyEvaluateOperator(random_info[id],
                GetPixelIndex(indexes+x),i == 0 ? AddEvaluateOperator : op,
                evaluate_pixel[x].index);
            p++;
          }
          image_view=DestroyCacheView(image_view);
          next=GetNextImageInList(next);
        }
        if (op == MeanEvaluateOperator)
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            evaluate_pixel[x].red/=number_images;
            evaluate_pixel[x].green/=number_images;
            evaluate_pixel[x].blue/=number_images;
            evaluate_pixel[x].opacity/=number_images;
            evaluate_pixel[x].index/=number_images;
          }
        if (op == RootMeanSquareEvaluateOperator)
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            evaluate_pixel[x].red=sqrt((double) evaluate_pixel[x].red/
              number_images);
            evaluate_pixel[x].green=sqrt((double) evaluate_pixel[x].green/
              number_images);
            evaluate_pixel[x].blue=sqrt((double) evaluate_pixel[x].blue/
              number_images);
            evaluate_pixel[x].opacity=sqrt((double) evaluate_pixel[x].opacity/
              number_images);
            evaluate_pixel[x].index=sqrt((double) evaluate_pixel[x].index/
              number_images);
          }
        if (op == MultiplyEvaluateOperator)
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            register ssize_t
              j;

            for (j=0; j < (ssize_t) (number_images-1); j++)
            {
              evaluate_pixel[x].red*=(MagickRealType) QuantumScale;
              evaluate_pixel[x].green*=(MagickRealType) QuantumScale;
              evaluate_pixel[x].blue*=(MagickRealType) QuantumScale;
              evaluate_pixel[x].opacity*=(MagickRealType) QuantumScale;
              evaluate_pixel[x].index*=(MagickRealType) QuantumScale;
            }
          }
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          SetPixelRed(q,ClampToQuantum(evaluate_pixel[x].red));
          SetPixelGreen(q,ClampToQuantum(evaluate_pixel[x].green));
          SetPixelBlue(q,ClampToQuantum(evaluate_pixel[x].blue));
          SetPixelAlpha(q,ClampToQuantum(evaluate_pixel[x].opacity));
          if (image->colorspace == CMYKColorspace)
            SetPixelIndex(evaluate_indexes+x,ClampToQuantum(
              evaluate_pixel[x].index));
          q++;
        }
        if (SyncCacheViewAuthenticPixels(evaluate_view,exception) == MagickFalse)
          status=MagickFalse;
        if (images->progress_monitor != (MagickProgressMonitor) NULL)
          {
            MagickBooleanType
              proceed;

            proceed=SetImageProgress(images,EvaluateImageTag,progress++,
              image->rows);
            if (proceed == MagickFalse)
              status=MagickFalse;
          }
      }
    }
  evaluate_view=DestroyCacheView(evaluate_view);
  evaluate_pixels=DestroyPixelThreadSet(images,evaluate_pixels);
  random_info=DestroyRandomInfoThreadSet(random_info);
  if (status == MagickFalse)
    image=DestroyImage(image);
  return(image);
}