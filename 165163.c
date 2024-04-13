static double RunOpenCLBenchmark(MagickBooleanType is_cpu)
{
  AccelerateTimer
    timer;

  ExceptionInfo
    *exception;

  Image
    *inputImage;

  ImageInfo
    *imageInfo;

  size_t
    i;

  exception=AcquireExceptionInfo();
  imageInfo=AcquireImageInfo();
  CloneString(&imageInfo->size,"2048x1536");
  CopyMagickString(imageInfo->filename,"xc:none",MagickPathExtent);
  inputImage=ReadImage(imageInfo,exception);

  InitAccelerateTimer(&timer);

  for (i=0; i<=2; i++)
  {
    Image
      *bluredImage,
      *resizedImage,
      *unsharpedImage;

    if (i > 0)
      StartAccelerateTimer(&timer);

    bluredImage=BlurImage(inputImage,10.0f,3.5f,exception);
    unsharpedImage=UnsharpMaskImage(bluredImage,2.0f,2.0f,50.0f,10.0f,
      exception);
    resizedImage=ResizeImage(unsharpedImage,640,480,LanczosFilter,
      exception);

    /* 
      We need this to get a proper performance benchmark, the operations
      are executed asynchronous.
    */
    if (is_cpu == MagickFalse)
      {
        CacheInfo
          *cache_info;

        cache_info=(CacheInfo *) resizedImage->cache;
        if (cache_info->opencl != (MagickCLCacheInfo) NULL)
          openCL_library->clWaitForEvents(cache_info->opencl->event_count,
            cache_info->opencl->events);
      }

    if (i > 0)
      StopAccelerateTimer(&timer);

    if (bluredImage != (Image *) NULL)
      DestroyImage(bluredImage);
    if (unsharpedImage != (Image *) NULL)
      DestroyImage(unsharpedImage);
    if (resizedImage != (Image *) NULL)
      DestroyImage(resizedImage);
  }
  DestroyImage(inputImage);
  return(ReadAccelerateTimer(&timer));
}