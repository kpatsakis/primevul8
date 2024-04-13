MagickExport Image *PolynomialImageChannel(const Image *images,
  const ChannelType channel,const size_t number_terms,const double *terms,
  ExceptionInfo *exception)
{
#define PolynomialImageTag  "Polynomial/Image"

  CacheView
    *polynomial_view;

  Image
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    **magick_restrict polynomial_pixels,
    zero;

  ssize_t
    y;

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
  polynomial_pixels=AcquirePixelThreadSet(images);
  if (polynomial_pixels == (MagickPixelPacket **) NULL)
    {
      image=DestroyImage(image);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",images->filename);
      return((Image *) NULL);
    }
  /*
    Polynomial image pixels.
  */
  status=MagickTrue;
  progress=0;
  GetMagickPixelPacket(images,&zero);
  polynomial_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
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
      *magick_restrict polynomial_indexes;

    register MagickPixelPacket
      *polynomial_pixel;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      i,
      x;

    size_t
      number_images;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(polynomial_view,0,y,image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    polynomial_indexes=GetCacheViewAuthenticIndexQueue(polynomial_view);
    polynomial_pixel=polynomial_pixels[id];
    for (x=0; x < (ssize_t) image->columns; x++)
      polynomial_pixel[x]=zero;
    next=images;
    number_images=GetImageListLength(images);
    for (i=0; i < (ssize_t) number_images; i++)
    {
      register const IndexPacket
        *indexes;

      register const PixelPacket
        *p;

      if (i >= (ssize_t) number_terms)
        break;
      image_view=AcquireVirtualCacheView(next,exception);
      p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        {
          image_view=DestroyCacheView(image_view);
          break;
        }
      indexes=GetCacheViewVirtualIndexQueue(image_view);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        double
          coefficient,
          degree;

        coefficient=terms[i << 1];
        degree=terms[(i << 1)+1];
        if ((channel & RedChannel) != 0)
          polynomial_pixel[x].red+=coefficient*pow(QuantumScale*p->red,degree);
        if ((channel & GreenChannel) != 0)
          polynomial_pixel[x].green+=coefficient*pow(QuantumScale*p->green,
            degree);
        if ((channel & BlueChannel) != 0)
          polynomial_pixel[x].blue+=coefficient*pow(QuantumScale*p->blue,
            degree);
        if ((channel & OpacityChannel) != 0)
          polynomial_pixel[x].opacity+=coefficient*pow(QuantumScale*
            (QuantumRange-p->opacity),degree);
        if (((channel & IndexChannel) != 0) &&
            (image->colorspace == CMYKColorspace))
          polynomial_pixel[x].index+=coefficient*pow(QuantumScale*indexes[x],
            degree);
        p++;
      }
      image_view=DestroyCacheView(image_view);
      next=GetNextImageInList(next);
    }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ClampToQuantum(QuantumRange*polynomial_pixel[x].red));
      SetPixelGreen(q,ClampToQuantum(QuantumRange*polynomial_pixel[x].green));
      SetPixelBlue(q,ClampToQuantum(QuantumRange*polynomial_pixel[x].blue));
      if (image->matte == MagickFalse)
        SetPixelOpacity(q,ClampToQuantum(QuantumRange-QuantumRange*
          polynomial_pixel[x].opacity));
      else
        SetPixelAlpha(q,ClampToQuantum(QuantumRange-QuantumRange*
          polynomial_pixel[x].opacity));
      if (image->colorspace == CMYKColorspace)
        SetPixelIndex(polynomial_indexes+x,ClampToQuantum(QuantumRange*
          polynomial_pixel[x].index));
      q++;
    }
    if (SyncCacheViewAuthenticPixels(polynomial_view,exception) == MagickFalse)
      status=MagickFalse;
    if (images->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(images,PolynomialImageTag,progress++,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  polynomial_view=DestroyCacheView(polynomial_view);
  polynomial_pixels=DestroyPixelThreadSet(images,polynomial_pixels);
  if (status == MagickFalse)
    image=DestroyImage(image);
  return(image);
}