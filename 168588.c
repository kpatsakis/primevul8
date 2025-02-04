static MagickBooleanType VerticalFilter(const ResizeFilter *resize_filter,
  const Image *image,Image *resize_image,const double y_factor,
  const MagickSizeType span,MagickOffsetType *offset,ExceptionInfo *exception)
{
  CacheView
    *image_view,
    *resize_view;

  ClassType
    storage_class;

  ContributionInfo
    **magick_restrict contributions;

  double
    scale,
    support;

  MagickBooleanType
    status;

  ssize_t
    y;

  /*
    Apply filter to resize vertically from image to resize image.
  */
  scale=MagickMax(1.0/y_factor+MagickEpsilon,1.0);
  support=scale*GetResizeFilterSupport(resize_filter);
  storage_class=support > 0.5 ? DirectClass : image->storage_class;
  if (SetImageStorageClass(resize_image,storage_class,exception) == MagickFalse)
    return(MagickFalse);
  if (support < 0.5)
    {
      /*
        Support too small even for nearest neighbour: Reduce to point sampling.
      */
      support=(double) 0.5;
      scale=1.0;
    }
  contributions=AcquireContributionThreadSet((size_t) (2.0*support+3.0));
  if (contributions == (ContributionInfo **) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(MagickFalse);
    }
  status=MagickTrue;
  scale=PerceptibleReciprocal(scale);
  image_view=AcquireVirtualCacheView(image,exception);
  resize_view=AcquireAuthenticCacheView(resize_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,resize_image,resize_image->rows,1)
#endif
  for (y=0; y < (ssize_t) resize_image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    double
      bisect,
      density;

    register const Quantum
      *magick_restrict p;

    register ContributionInfo
      *magick_restrict contribution;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    ssize_t
      n,
      start,
      stop;

    if (status == MagickFalse)
      continue;
    bisect=(double) (y+0.5)/y_factor+MagickEpsilon;
    start=(ssize_t) MagickMax(bisect-support+0.5,0.0);
    stop=(ssize_t) MagickMin(bisect+support+0.5,(double) image->rows);
    density=0.0;
    contribution=contributions[id];
    for (n=0; n < (stop-start); n++)
    {
      contribution[n].pixel=start+n;
      contribution[n].weight=GetResizeFilterWeight(resize_filter,scale*
        ((double) (start+n)-bisect+0.5));
      density+=contribution[n].weight;
    }
    if (n == 0)
      continue;
    if ((density != 0.0) && (density != 1.0))
      {
        register ssize_t
          i;

        /*
          Normalize.
        */
        density=PerceptibleReciprocal(density);
        for (i=0; i < n; i++)
          contribution[i].weight*=density;
      }
    p=GetCacheViewVirtualPixels(image_view,0,contribution[0].pixel,
      image->columns,(size_t) (contribution[n-1].pixel-contribution[0].pixel+1),
      exception);
    q=QueueCacheViewAuthenticPixels(resize_view,0,y,resize_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) resize_image->columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        double
          alpha,
          gamma,
          pixel;

        PixelChannel
          channel;

        PixelTrait
          resize_traits,
          traits;

        register ssize_t
          j;

        ssize_t
          k;

        channel=GetPixelChannelChannel(image,i);
        traits=GetPixelChannelTraits(image,channel);
        resize_traits=GetPixelChannelTraits(resize_image,channel);
        if ((traits == UndefinedPixelTrait) ||
            (resize_traits == UndefinedPixelTrait))
          continue;
        if (((resize_traits & CopyPixelTrait) != 0) ||
            (GetPixelWriteMask(resize_image,q) <= (QuantumRange/2)))
          {
            j=(ssize_t) (MagickMin(MagickMax(bisect,(double) start),(double)
              stop-1.0)+0.5);
            k=(ssize_t) ((contribution[j-start].pixel-contribution[0].pixel)*
              image->columns+x);
            SetPixelChannel(resize_image,channel,p[k*GetPixelChannels(image)+i],
              q);
            continue;
          }
        pixel=0.0;
        if ((resize_traits & BlendPixelTrait) == 0)
          {
            /*
              No alpha blending.
            */
            for (j=0; j < n; j++)
            {
              k=(ssize_t) ((contribution[j].pixel-contribution[0].pixel)*
                image->columns+x);
              alpha=contribution[j].weight;
              pixel+=alpha*p[k*GetPixelChannels(image)+i];
            }
            SetPixelChannel(resize_image,channel,ClampToQuantum(pixel),q);
            continue;
          }
        gamma=0.0;
        for (j=0; j < n; j++)
        {
          k=(ssize_t) ((contribution[j].pixel-contribution[0].pixel)*
            image->columns+x);
          alpha=contribution[j].weight*QuantumScale*GetPixelAlpha(image,p+k*
            GetPixelChannels(image));
          pixel+=alpha*p[k*GetPixelChannels(image)+i];
          gamma+=alpha;
        }
        gamma=PerceptibleReciprocal(gamma);
        SetPixelChannel(resize_image,channel,ClampToQuantum(gamma*pixel),q);
      }
      q+=GetPixelChannels(resize_image);
    }
    if (SyncCacheViewAuthenticPixels(resize_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_VerticalFilter)
#endif
        proceed=SetImageProgress(image,ResizeImageTag,(*offset)++,span);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  resize_view=DestroyCacheView(resize_view);
  image_view=DestroyCacheView(image_view);
  contributions=DestroyContributionThreadSet(contributions);
  return(status);
}